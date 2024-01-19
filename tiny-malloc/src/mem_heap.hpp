#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include "mem_chunk.hpp"

#include <memory>

class mem_node_t
{
public:
    std::shared_ptr<mem_node_t> next;
    mem_chunk_t chunk;

    mem_node_t() = default;

    explicit mem_node_t(mem_chunk_t chunk) : chunk(chunk)
    {
    }

    ~mem_node_t() = default;
};

class mem_heap_t
{
public:
    std::shared_ptr<mem_node_t> head;

    mem_heap_t() = default;

    ~mem_heap_t() = default;

    std::shared_ptr<mem_node_t> add(void* ptr, std::size_t size)
    {
        auto new_node = std::make_shared<mem_node_t>(mem_chunk_t(ptr, size));
        new_node->next = head;
        head = new_node;
        return new_node;
    }

    std::shared_ptr<mem_node_t> find(void* ptr)
    {
        auto current = head;
        while (current != nullptr)
        {
            if (current->chunk.ptr == ptr)
            {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    bool remove(void* ptr)
    {
        auto current = head;
        std::shared_ptr<mem_node_t> previous = nullptr;
        while (current != nullptr)
        {
            if (current->chunk.ptr == ptr)
            {
                if (previous != nullptr)
                {
                    previous->next = current->next;
                }
                else
                {
                    head = current->next;
                }
                return true;
            }
            previous = current;
            current = current->next;
        }
        return false;
    }
};

#endif //LINKED_LIST_H
