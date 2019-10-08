//
// Created by Zh1an on 2019/10/8.
//

#include "timer.hpp"
#include "config.h"

#include <algorithm>

timer::timer() : m_timer_id(0) {
#if USE_CPP_MIN_HEAP_TIMER
    std::make_heap(m_timer.begin(), m_timer.end(), ev_cmp);
#elif USE_CPP_SORTED_TIMER

#endif
}

timer::~timer() {
    m_timer.clear();
}

unsigned timer::timerAdd(int interval, ev_callback_t cb, int flag, int exe_num) {
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

    m_timer.push_back(ev);
#if USE_CPP_MIN_HEAP_TIMER
    std::push_heap(m_timer.begin(), m_timer.end(), ev_cmp);
#elif USE_CPP_SORTED_TIMER
    std::sort(m_timer.begin(), m_timer.end(), ev_cmp);
#endif
    return ev.timer_id;
}

bool timer::timerRemove(unsigned timer_id) {
    auto iter = m_timer.begin();
    for (; iter != m_timer.end();) {
        if (timer_id == (*iter).timer_id) {
            iter = m_timer.erase(iter);
#if USE_CPP_MIN_HEAP_TIMER
            std::push_heap(m_timer.begin(), m_timer.end(), ev_cmp);
#elif USE_CPP_SORTED_TIMER
            std::sort(m_timer.begin(), m_timer.end(), ev_cmp);
#endif
            return true;
        } else {
            ++iter;
        }
    }

    return false;
}

int timer::timerProcess() {
    if (m_timer.empty())
        return -1;

    struct timeval now = {0, 0};
    auto iter = m_timer.begin();
    for (; iter != m_timer.end();) {
        getTime(&now);
        if (evutil_timercmp(&now, &((*iter).ev_timeout), <))
            break;
        if ((*iter).ev_callback)
            (*iter).ev_callback();
        if ((*iter).ev_flag == ABSOLUTE_TIMER || ((*iter).ev_flag == RELATIVE_TIMER && --(*iter).ev_exe_num) > 0) {
            evutil_timeradd(&((*iter).ev_timeout), &((*iter).ev_interval), &((*iter).ev_timeout));
            ++iter;
        } else {
            iter = m_timer.erase(iter);
        }

    }
#if  USE_CPP_MIN_HEAP_TIMER
    std::push_heap(m_timer.begin(), m_timer.end(), ev_cmp);
#elif USE_CPP_SORTED_TIMER
    std::sort(m_timer.begin(), m_timer.end(), ev_cmp);
#endif
    return 0;
}

struct timeval timer::getTopElapsedTimeval() {
    struct timeval s = {0, 0};
    if (m_timer.empty())
        return s;

    auto &ev = m_timer.front();
    struct timeval now = {0, 0};
    getTime(&now);
    if (evutil_timercmp(&now, &(ev.ev_timeout), <)) {
        s.tv_sec = ev.ev_timeout.tv_sec - now.tv_sec;
        s.tv_usec = ev.ev_timeout.tv_usec - now.tv_usec;
    }

    return s;
}
