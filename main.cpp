
#include <vector>
#include <iostream>
#include "InfiniAllocator.h"
#include "InfiniTracer.h"
#include <Clock.h>
#include <fstream>
#include "SimpleVectorTimeline.h"

using json = nlohmann::json;

int main()
{
    Clock system_clock;

    std::vector<std::unique_ptr<TimelineStrategy>> strategies;
    strategies.push_back(std::unique_ptr<TimelineStrategy>(reinterpret_cast<TimelineStrategy*>(new SimpleVectorTimeline(system_clock))));

    // 1. Read sample file
    std::ifstream f("weather_log.json");
    json weather_data = json::parse(f);

    // 2. For each strategem
    for(auto& strategy : strategies)
    {
        std::cout << "===============================================================================\n";
        std::cout << "Strategy: " << strategy->name() << "\n";

        // 2.1 For each entry feed the strategem (adjust the current time)
        for (auto& weather_entry : weather_data) {
            system_clock.set_current_time(weather_entry["clock"]);

            strategy->on_insert_event(weather_entry["data"]);
        }

        // 2.2 Dump the metrics
        InfiniTracer::dump();
        std::cout << "Size:          " << strategy->event_count()  << "\n";
        std::cout << "Capacity:      " << strategy->event_capacity() << "\n";

        // 2.3 Destroy the strategem
        strategy.reset();

        bool is_leaking = InfiniTracer::is_leaking();
        // 2.4 Check for leak 
        std::cout << "Leaking:       " << (is_leaking ? "true\n" : "false\n");
        if(is_leaking){
            auto leaks = InfiniTracer::allocs_without_corresponding_free();
            for(const auto& leak : leaks){
                std::cout << "Ptr: " << leak.ptr << " Size: " << leak.size << "\n";
            }
        }
        std::cout << "===============================================================================\n";

        // 2.5 Reset tracer && Memory Pool
        InfiniTracer::the().reset();
        reset_mempool();
    }
}