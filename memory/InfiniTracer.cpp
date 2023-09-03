#include "InfiniTracer.h"
#include "heap_4_infinitime.h"
#include <iostream>
#include <algorithm>
#include "FreeRTOSConfig.h"

static InfiniTracer* tracer;

InfiniTracer &InfiniTracer::the() { 
    if(!tracer){
        tracer = new InfiniTracer();
    }
    return *tracer;
}

double InfiniTracer::fragmentation() 
{
    size_t largest_block = xPortGetLargestFreeBlock();
    if(largest_block == 0){
        return 0;
    }
    return 1.0 - ((double)largest_block / (double)xPortGetFreeHeapSize());
}

size_t InfiniTracer::lowest_free_size(){
    return xPortGetMinimumEverFreeHeapSize();
}

void InfiniTracer::dump() {
    std::cout << "Allocations:   " << the().m_allocs.size() << "\n";
    std::cout << "Deallocations: " << the().m_frees.size() << "\n";
    std::cout << "OOM:           " << the().m_oom_count << "\n";
    std::cout << "Lowest Ever:   " << lowest_free_size() << "\n";
    std::cout << "Max Ever:      " << configTOTAL_HEAP_SIZE - lowest_free_size() << "\n";
    std::cout << "Fragmentation: " << fragmentation() << "\n";
}

void InfiniTracer::trace_alloc(Allocation& alloc){
    auto& self = InfiniTracer::the();
    self.m_allocs.push_back(std::move(alloc));
}

void InfiniTracer::trace_free(Allocation& alloc) {
    auto& self = InfiniTracer::the();
    self.m_frees.push_back(std::move(alloc));
}

bool InfiniTracer::is_leaking(){
   auto leaks = InfiniTracer::allocs_without_corresponding_free();
   return leaks.size() > 0;
}

std::vector<Allocation> InfiniTracer::allocs_without_corresponding_free() {
    auto& self = InfiniTracer::the();
    std::vector<Allocation> found_leaks;

    for(const auto& mallocation : self.m_allocs){
        auto it = std::find_if(self.m_frees.begin(), self.m_frees.end(), [&](const Allocation& alloc){
            // Ignore size equality
            return mallocation.ptr == alloc.ptr;
        });
        if(it == self.m_frees.end()){
            found_leaks.push_back(mallocation);
        }
    }
    return found_leaks;
}

void InfiniTracer::reset() {
    auto& self = InfiniTracer::the();

    self.m_oom_count = 0;
    self.m_frees.clear();
    self.m_allocs.clear();
}

void vApplicationMallocFailedHook( void ){
   InfiniTracer::the().m_oom_count++;
}

void traceMALLOC(void* pt, size_t size)
{
    Allocation alloc {pt, size};
    InfiniTracer::the().trace_alloc(alloc);
}

void traceFREE(void* pt, size_t size){
    Allocation alloc {pt, size};
    InfiniTracer::the().trace_free(alloc);
}

