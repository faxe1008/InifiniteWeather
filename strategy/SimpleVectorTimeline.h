#pragma once

#include "TimelineStrategy.h"
#include "InfiniAllocator.h"
#include "WeatherData.h"

class SimpleVectorTimeline : public TimelineStrategy {
public:
    SimpleVectorTimeline(const Clock& clock) : 
        m_clock(clock)
    {
    }

    virtual ~SimpleVectorTimeline() {
    }

    virtual const char* name() {
        return "SimpleVectorTimeline";
    }

    virtual bool on_insert_event(const nlohmann::json& json_object);

    virtual size_t event_count() const
    {
        return m_events.size();
    }

    virtual size_t event_capacity() const {
        return m_events.capacity();
    }

private:
    bool AddEventToTimeline(pool_unique_ptr<WeatherData::TimelineHeader> event);
    void TidyTimeline();
    uint64_t GetCurrentUnixTimestamp() const;
    bool IsEventStillValid(const pool_unique_ptr<WeatherData::TimelineHeader>& uniquePtr, const uint64_t timestamp);


    const Clock& m_clock;
    std::vector<pool_unique_ptr<WeatherData::TimelineHeader>, infini_allocator<WeatherData::TimelineHeader>> m_events;
};