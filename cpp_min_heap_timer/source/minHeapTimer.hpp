//
// Created by Zh1an on 2019/9/29.
//

#ifndef CPP_MINHEAPTIMER_HPP
#define CPP_MINHEAPTIMER_HPP

#include <functional>
#include <vector>

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


class minHeapTimer {
public:
    minHeapTimer();

    virtual ~minHeapTimer();

public:
    unsigned timerAdd(int interval, ev_callback_t cb, int flag = ABSOLUTE_TIMER, int exe_num = 0);

    bool timerRemove(unsigned timer_id);

    int timerProcess();

public:
    struct timeval getTopElapsedTimeval();

private:
    std::vector<ev_event_t> m_min_heap_timer;
    unsigned m_timer_id;
};


#endif //CPP_MINHEAPTIMER_HPP
