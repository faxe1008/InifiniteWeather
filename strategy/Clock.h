#pragma once
#include <stdint.h>

class Clock {
public:

    int64_t get_current_time() const {
        return m_secs_since_epoch;
    }

    void set_current_time(int64_t time) {
        m_secs_since_epoch = time;
    }

private:
    int64_t m_secs_since_epoch { 0 };
};