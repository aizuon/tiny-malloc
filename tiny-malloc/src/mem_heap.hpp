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
        auto new_block = std::make_shared<mem_block_t>(size);
        if (!new_block->ptr)
        {
            return nullptr;
        }
        new_block->next_block = blocks_head;
        blocks_head = new_block;
        return new_block;
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
                    prev->next_block = current->next_block;
                }
                else
                {
                    blocks_head = current->next_block;
                }

                current = (prev) ? prev->next_block : blocks_head;
            }
            else
            {
                prev = current;
                current = current->next_block;
            }
        }
    }

    void add_to_free_list(std::shared_ptr<mem_chunk_t> node)
    {
        node->next = nullptr;

        if (!free_list_head || node < free_list_head)
        {
            node->next = free_list_head;
            free_list_head = node;
        }
        else
        {
            auto current = free_list_head;
            while (current->next && current->next->ptr < node->ptr)
            {
                current = current->next;
            }
            node->next = current->next;
            current->next = node;
        }
    }

    void remove_from_free_list(std::shared_ptr<mem_chunk_t> node)
    {
        if (!free_list_head)
        {
            return;
        }

        if (free_list_head == node)
        {
            free_list_head = node->next;
            node->next = nullptr;
            return;
        }

        auto current = free_list_head;
        std::shared_ptr<mem_chunk_t> prev = nullptr;
        while (current != node && current->next)
        {
            prev = current;
            current = current->next;
        }

        if (current == node)
        {
            if (prev)
            {
                prev->next = current->next;
            }
            node->next = nullptr;
        }
    }

    void coalesce_free_list()
    {
        auto current = free_list_head;
        while (current && current->next)
        {
            auto next = current->next;
            if (reinterpret_cast<char*>(current->ptr) + current->size == next->ptr)
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
        std::shared_ptr<mem_chunk_t> prev = nullptr;
        while (current)
        {
            if (current->size >= size)
            {
                if (current->size > size + sizeof(mem_chunk_t))
                {
                    auto remaining_size = current->size - size - sizeof(mem_chunk_t);
                    auto new_chunk_ptr = reinterpret_cast<char*>(current->ptr) + size + sizeof(mem_chunk_t);
                    auto new_chunk = std::make_shared<mem_chunk_t>(new_chunk_ptr, remaining_size, true);
                    current->size = size;
                    new_chunk->next = current->next;
                    current->next = new_chunk;
                    new_chunk->is_free = true;
                }
                if (prev)
                {
                    prev->next = current->next;
                }
                else
                {
                    free_list_head = current->next;
                }
                current->is_free = false;
                current->next = nullptr;
                if (current->size > size)
                {
                    add_to_free_list(current->next);
                }
                return current;
            }
            prev = current;
            current = current->next;
        }
        return nullptr;
    }

    std::shared_ptr<mem_chunk_t> allocate_chunk(std::size_t size)
    {
        auto chunk = find_free_chunk(size);
        if (chunk)
        {
            return chunk;
        }

        coalesce_free_list();

        chunk = find_free_chunk(size);
        if (chunk)
        {
            return chunk;
        }

        std::size_t block_size = std::max(size + sizeof(mem_chunk_t),
                                          static_cast<std::size_t>(sysconf(_SC_PAGESIZE) * 16));
        auto new_block = allocate_block(block_size);
        if (!new_block)
        {
            return nullptr;
        }

        auto new_chunk = std::make_shared<mem_chunk_t>(new_block->ptr, block_size, true);
        new_block->chunks_head = new_chunk;
        add_to_free_list(new_chunk);

        return find_free_chunk(size);
    }

    void free_chunk(std::shared_ptr<mem_chunk_t> node)
    {
        node->is_free = true;
        add_to_free_list(node);
        coalesce_free_list();
        free_unused_blocks();
    }
};

#endif //MEM_HEAP_H
