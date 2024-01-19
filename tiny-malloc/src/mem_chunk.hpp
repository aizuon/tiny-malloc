#ifndef MEM_BLOCK_H
#define MEM_BLOCK_H

#include <cstddef>

class mem_chunk_t
{
public:
    void* ptr;
    std::size_t size;

    mem_chunk_t() = default;

    mem_chunk_t(void* ptr, std::size_t size) : ptr(ptr), size(size)
    {
    }
};

#endif //MEM_BLOCK_H
