//
// Created by Zh1an on 2019/9/29.
//

#include "minHeapTimer.hpp"

#include <algorithm>

#define evutil_timercmp(tvp, uvp, cmp) \
  (((tvp)->tv_sec == (uvp)->tv_sec) ? ((tvp)->tv_usec cmp(uvp)->tv_usec) : ((tvp)->tv_sec cmp(uvp)->tv_sec))

#define evutil_timeradd(tvp, uvp, vvp)                \
  do                                                  \
  {                                                   \
    (vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;    \
    (vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec; \
    if ((vvp)->tv_usec >= 1000000)                    \
    {                                                 \
      (vvp)->tv_sec++;                                \
      (vvp)->tv_usec -= 1000000;                      \
    }                                                 \
  } while (0)

#ifdef _WIN32

int gettimeofday(struct timeval *tv, void *attr) {
    union {
        long long ns100;
        FILETIME ft;
    } now;

    GetSystemTimeAsFileTime(&now.ft);
    tv->tv_usec = (long) ((now.ns100 / 10LL) % 1000000LL);
    tv->tv_sec = (long) ((now.ns100 - 116444736000000000LL) / 10000000LL);
    return (0);
}

#endif

static inline void getTime(struct timeval *tm) {
    gettimeofday(tm, NULL);
}

bool ev_cmp(const ev_event_t &a, const ev_event_t &b) {
    return evutil_timercmp(&(a.ev_timeout), &(a.ev_timeout), >);
}

minHeapTimer::minHeapTimer() : m_timer_id(0) {
    std::make_heap(m_min_heap_timer.begin(), m_min_heap_timer.end(), ev_cmp);
}

minHeapTimer::~minHeapTimer() {
    m_min_heap_timer.clear();
}

unsigned minHeapTimer::timerAdd(int interval, ev_callback_t cb, int flag, int exe_num) {
    ev_event_t ev;
    struct timeval now = {0, 0};
    getTime(&now);

    ev.ev_interval.tv_usec = (interval % 1000) * 1000;
    ev.ev_interval.tv_sec = interval / 1000;
    evutil_timeradd(&now, &(ev.ev_interval), &(ev.ev_timeout));
    ev.ev_flag = flag;
    ev.ev_exe_num = exe_num;
    ev.timer_id = m_timer_id++;
    ev.ev_callback = std::move(cb);

    m_min_heap_timer.push_back(ev);
    std::push_heap(m_min_heap_timer.begin(), m_min_heap_timer.end(), ev_cmp);

    return ev.timer_id;
}

bool minHeapTimer::timerRemove(unsigned timer_id) {
    auto iter = m_min_heap_timer.begin();
    bool isNeedRemove = false;
    for (; iter != m_min_heap_timer.end(); ++iter) {
        if (timer_id == (*iter).timer_id) {
            isNeedRemove = true;
            break;
        }
    }


    if (isNeedRemove) {
        m_min_heap_timer.erase(iter);
        std::push_heap(m_min_heap_timer.begin(), m_min_heap_timer.end(), ev_cmp);
    }

    return isNeedRemove;
}

int minHeapTimer::timerProcess() {
    if (m_min_heap_timer.empty())
        return -1;

    struct timeval now = {0, 0};
    std::vector<std::vector<ev_event_t>::iterator> s;
    auto tmp = m_min_heap_timer.begin();
    for (auto &iter : m_min_heap_timer) {
        getTime(&now);
        if (evutil_timercmp(&now, &(iter.ev_timeout), <))
            break;

        if (iter.ev_callback)
            iter.ev_callback();
        if (iter.ev_flag == ABSOLUTE_TIMER || (iter.ev_flag == RELATIVE_TIMER && --iter.ev_exe_num > 0))
            evutil_timeradd(&(iter.ev_timeout), &(iter.ev_interval), &(iter.ev_timeout));
        else
            s.push_back(tmp++);
    }

    if (!s.empty()) {
        for (auto iter : s)
            m_min_heap_timer.erase(iter);
    }
    std::push_heap(m_min_heap_timer.begin(), m_min_heap_timer.end(), ev_cmp);

    return 0;
}

struct timeval minHeapTimer::getTopElapsedTimeval() {
    struct timeval s = {0, 0};
    if (m_min_heap_timer.empty())
        return s;

    auto &ev = m_min_heap_timer.front();
    struct timeval now = {0, 0};
    getTime(&now);
    if (evutil_timercmp(&now, &(ev.ev_timeout), <)) {
        s.tv_sec = ev.ev_timeout.tv_sec - now.tv_sec;
        s.tv_usec = ev.ev_timeout.tv_usec - now.tv_usec;
    }

    return s;
}

