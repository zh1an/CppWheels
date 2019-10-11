//
// Created by Zh1an on 2019/10/8.
//

#ifndef CPP_MIN_HEAP_TIMER_TIMER_HPP
#define CPP_MIN_HEAP_TIMER_TIMER_HPP

#include <vector>

#include "data.hpp"


class timer {
public:
    timer();

    virtual ~timer();

public:
    unsigned timerAdd(int interval, ev_callback_t cb, int flag = ABSOLUTE_TIMER, int exe_num = 0);

    bool timerRemove(unsigned timer_id);

    int timerProcess();

public:
    struct timeval getTopElapsedTimeval();

private:
    std::vector<ev_event_t> m_timer;
    unsigned m_timer_id;

};


#endif //CPP_MIN_HEAP_TIMER_TIMER_HPP
