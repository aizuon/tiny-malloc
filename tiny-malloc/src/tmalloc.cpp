#include "pch.hpp"
#include "mem_heap.hpp"
#include "mem_chunk.hpp"

#include <mutex>

std::mutex mutex;

auto heap = std::make_shared<mem_heap_t>();

void* tmalloc(std::size_t size)
{
    std::lock_guard lock(mutex);

    auto chunk = heap->allocate_chunk(size);
    if (!chunk)
    {
        return nullptr;
    }
    return chunk->ptr;
}

bool tfree(void* ptr)
{
    std::lock_guard lock(mutex);

    auto block = heap->blocks_head;
    while (block)
    {
        auto chunk = block->chunks_head;
        while (chunk)
        {
            if (chunk->ptr == ptr && !chunk->is_free)
            {
                heap->free_chunk(chunk);
                return true;
            }
            chunk = chunk->next;
        }
        block = block->next_block;
    }
    return false;
}
