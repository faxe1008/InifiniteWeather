#pragma once

#include <stdlib.h>
#include <vector>

struct Allocation {
    void* ptr;
    size_t size;
};

class InfiniTracer {
public:
    static InfiniTracer& the();

    static double fragmentation();
    static size_t lowest_free_size();

    static bool is_leaking();
    static std::vector<Allocation> allocs_without_corresponding_free();

    static void dump();
    static void reset();

    void trace_alloc(Allocation& alloc);
    void trace_free(Allocation& alloc);

    size_t m_oom_count {0};
private:
    InfiniTracer() {}

    std::vector<Allocation> m_allocs;
    std::vector<Allocation> m_frees;
};