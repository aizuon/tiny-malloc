#include "pch.hpp"
#include "mem_heap.hpp"
#include "mem_chunk.hpp"

#include <sys/mman.h>
#include <unistd.h>
#include <utility>

auto heap = std::make_shared<mem_heap_t>();

long page_size = sysconf(_SC_PAGESIZE);

void* tmalloc(std::size_t size)
{
    std::size_t pages_needed = (size + page_size - 1) / page_size;

    std::size_t total_size = pages_needed * page_size;

    void* ptr = mmap(
        nullptr,
        total_size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANON,
        -1,
        0
    );

    if (ptr == MAP_FAILED)
    {
        return nullptr;
    }

    heap->add(ptr, total_size);

    return ptr;
}

bool tfree(void* ptr)
{
    auto chunk = heap->find(ptr);
    if (chunk == nullptr)
    {
        return false;
    }

    return munmap(chunk->chunk.ptr, chunk->chunk.size) == 0;
}
