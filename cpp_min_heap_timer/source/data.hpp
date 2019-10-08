//
// Created by Zh1an on 2019/10/8.
//

#ifndef CPP_MIN_HEAP_TIMER_DATA_HPP
#define CPP_MIN_HEAP_TIMER_DATA_HPP

#include <functional>

#ifdef  _WIN32

#include <windows.h>

#else
#include <sys/time.h>
#endif

typedef std::function<void(void)> ev_callback_t;

struct ev_event_t {
    unsigned timer_id;
    ev_callback_t ev_callback;

#define RELATIVE_TIMER 1
#define ABSOLUTE_TIMER 2
    int ev_flag;
    int ev_exe_num;

    struct timeval ev_interval;
    struct timeval ev_timeout;
};

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

static int gettimeofday(struct timeval *tv, void *attr) {
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

static bool ev_cmp(const ev_event_t &a, const ev_event_t &b) {
    return evutil_timercmp(&(a.ev_timeout), &(a.ev_timeout), >);
}

#endif //CPP_MIN_HEAP_TIMER_DATA_HPP
