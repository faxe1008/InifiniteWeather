#pragma once

#include <Clock.h>
#include <nlohmann/json.hpp>

class TimelineStrategy
{
public:
    virtual ~TimelineStrategy() {}

    virtual const char* name() = 0;
    virtual bool on_insert_event(const nlohmann::json& json_object) = 0;
    virtual size_t event_count() const = 0;
    virtual size_t event_capacity() const = 0;
};