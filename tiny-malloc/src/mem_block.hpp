#ifndef MEM_BLOCK_H
#define MEM_BLOCK_H

#include "mem_chunk.hpp"

#include <cstddef>
#include <memory>
#include <sys/mman.h>

class mem_block_t
{
public:
    void* ptr;
    std::size_t size;

    std::shared_ptr<mem_block_t> next_block;
    std::shared_ptr<mem_chunk_t> chunks_head;

    explicit mem_block_t(std::size_t size)
        : size(size), next_block(nullptr), chunks_head(nullptr)
    {
        ptr = mmap(
            nullptr,
            size,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANON,
            -1,
            0
        );

        if (ptr == MAP_FAILED)
        {
            ptr = nullptr;
        }
    }

    ~mem_block_t()
    {
        munmap(ptr, size);
    }

    bool all_chunks_free() const
    {
        auto chunk = chunks_head;
        while (chunk)
        {
            if (!chunk->is_free)
            {
                return false;
            }
            chunk = chunk->next;
        }
        return true;
    }
};

#endif //MEM_BLOCK_H
