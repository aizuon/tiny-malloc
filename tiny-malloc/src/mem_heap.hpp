#ifndef MEM_HEAP_H
#define MEM_HEAP_H

#include "mem_chunk.hpp"
#include "mem_block.hpp"

#include <memory>
#include <algorithm>
#include <unistd.h>

class mem_heap_t
{
public:
    std::shared_ptr<mem_block_t> blocks_head;
    std::shared_ptr<mem_chunk_t> free_list_head;

    mem_heap_t() = default;

    ~mem_heap_t() = default;

    std::shared_ptr<mem_block_t> allocate_block(std::size_t size)
    {
        auto block = std::make_shared<mem_block_t>(size);
        if (!block->ptr)
        {
            return nullptr;
        }
        block->next = blocks_head;
        blocks_head = block;
        return block;
    }

    void free_unused_blocks()
    {
        std::shared_ptr<mem_block_t> current = blocks_head;
        std::shared_ptr<mem_block_t> prev = nullptr;

        while (current)
        {
            if (current->all_chunks_free())
            {
                if (prev)
                {
                    prev->next = current->next;
                }
                else
                {
                    blocks_head = current->next;
                }

                current = prev ? prev->next : blocks_head;
            }
            else
            {
                prev = current;
                current = current->next;
            }
        }
    }

    void add_to_free_list(std::shared_ptr<mem_chunk_t> chunk)
    {
        chunk->is_free = true;

        if (!free_list_head)
        {
            chunk->next = nullptr;
        }
        else
        {
            chunk->next = free_list_head;
        }
        free_list_head = chunk;
    }

    void remove_from_free_list(std::shared_ptr<mem_chunk_t> chunk)
    {
        if (!free_list_head)
        {
            return;
        }

        chunk->is_free = false;

        if (chunk == free_list_head)
        {
            free_list_head = chunk->next;
            chunk->next = nullptr;
            return;
        }

        auto current = free_list_head;
        std::shared_ptr<mem_chunk_t> prev = nullptr;
        while (current != chunk && current->next)
        {
            prev = current;
            current = current->next;
        }

        if (current == chunk)
        {
            if (prev)
            {
                prev->next = current->next;
            }
            chunk->next = nullptr;
        }
    }

    void coalesce_free_list()
    {
        auto current = free_list_head;
        while (current && current->next)
        {
            auto next = current->next;
            if (static_cast<char*>(current->ptr) + current->size == next->ptr)
            {
                current->size += next->size;
                current->next = next->next;
            }
            else
            {
                current = next;
            }
        }
    }

    std::shared_ptr<mem_chunk_t> find_free_chunk(std::size_t size)
    {
        auto current = free_list_head;
        while (current)
        {
            if (current->size >= size)
            {
                if (current->size > size)
                {
                    auto remaining_size = current->size - size;
                    auto new_chunk_ptr = static_cast<char*>(current->ptr) + size;
                    auto new_chunk = std::make_shared<mem_chunk_t>(new_chunk_ptr, remaining_size);
                    current->size = size;
                    add_to_free_list(new_chunk);
                }
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    std::shared_ptr<mem_chunk_t> allocate_chunk(std::size_t size)
    {
        auto chunk = find_free_chunk(size);
        if (chunk)
        {
            remove_from_free_list(chunk);
            return chunk;
        }

        coalesce_free_list();

        chunk = find_free_chunk(size);
        if (chunk)
        {
            remove_from_free_list(chunk);
            return chunk;
        }

        std::size_t block_size = std::max(static_cast<std::size_t>(size * 1.5f),
                                          static_cast<std::size_t>(sysconf(_SC_PAGESIZE) * 16));
        auto new_block = allocate_block(block_size);
        if (!new_block)
        {
            return nullptr;
        }

        auto new_chunk = std::make_shared<mem_chunk_t>(new_block->ptr, block_size);
        new_block->chunks_head = new_chunk;
        add_to_free_list(new_chunk);

        return find_free_chunk(size);
    }

    void free_chunk(std::shared_ptr<mem_chunk_t> chunk)
    {
        add_to_free_list(chunk);
        static size_t free_count = 0;
        free_count++;
        if (free_count % 256 == 0)
        {
            coalesce_free_list();
        }
        free_unused_blocks();
    }
};

#endif //MEM_HEAP_H
