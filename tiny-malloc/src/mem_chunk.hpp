#ifndef MEM_CHUNK_H
#define MEM_CHUNK_H

#include <cstddef>

class mem_chunk_t
{
public:
    void* ptr;
    std::size_t size;
    bool is_free;

    std::shared_ptr<mem_chunk_t> next;

    mem_chunk_t() : ptr(nullptr), size(0), is_free(true)
    {
    }

    mem_chunk_t(void* ptr, std::size_t size, bool is_free = true)
        : ptr(ptr), size(size), is_free(is_free)
    {
    }
};

#endif //MEM_CHUNK_H
