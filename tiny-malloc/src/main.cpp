#include "tmalloc.hpp"

#include <cstdlib>
#include <iostream>
#include <chrono>
#include <vector>
#include <ranges>
#include <algorithm>
#include <random>

int main(int argc, char** argv)
{
    const size_t allocations = 10;
    const size_t min_alloc_size = 128;
    const size_t max_alloc_size = 4096;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(min_alloc_size, max_alloc_size);

    auto start_malloc = std::chrono::high_resolution_clock::now();
    auto start_malloc_allocation = std::chrono::high_resolution_clock::now();
    std::vector<void*> malloc_pointers;
    for (size_t i = 0; i < allocations; ++i)
    {
        size_t alloc_size = dist(gen);
        void* ptr = malloc(alloc_size);
        malloc_pointers.push_back(ptr);
    }
    auto end_malloc_allocation = std::chrono::high_resolution_clock::now();
    auto duration_malloc_allocation = std::chrono::duration_cast<std::chrono::microseconds>(
        end_malloc_allocation - start_malloc_allocation).count();
    std::cout << "Standard malloc time: " << duration_malloc_allocation << " microseconds" << std::endl;
    auto start_malloc_deallocation = std::chrono::high_resolution_clock::now();
    std::ranges::shuffle(malloc_pointers, gen);
    for (void* ptr: malloc_pointers)
    {
        free(ptr);
    }
    auto end_malloc_deallocation = std::chrono::high_resolution_clock::now();
    auto duration_malloc_deallocation = std::chrono::duration_cast<std::chrono::microseconds>(
        end_malloc_deallocation - start_malloc_deallocation).count();
    std::cout << "Standard malloc deallocation time: " << duration_malloc_deallocation << " microseconds" << std::endl;
    auto end_malloc = std::chrono::high_resolution_clock::now();
    auto duration_malloc = std::chrono::duration_cast<std::chrono::microseconds>(end_malloc - start_malloc).count();
    std::cout << "Standard malloc time (total): " << duration_malloc << " microseconds" << std::endl;

    auto start_custom = std::chrono::high_resolution_clock::now();
    auto start_custom_allocation = std::chrono::high_resolution_clock::now();
    std::vector<void*> custom_pointers;
    for (size_t i = 0; i < allocations; ++i)
    {
        size_t alloc_size = dist(gen);
        void* ptr = tmalloc(alloc_size);
        custom_pointers.push_back(ptr);
    }
    auto end_custom_allocation = std::chrono::high_resolution_clock::now();
    auto duration_custom_allocation = std::chrono::duration_cast<std::chrono::microseconds>(
        end_custom_allocation - start_custom_allocation).count();
    std::cout << "Custom allocator allocation time: " << duration_custom_allocation << " microseconds" << std::endl;
    auto start_custom_deallocation = std::chrono::high_resolution_clock::now();
    std::ranges::shuffle(custom_pointers, gen);
    for (void* ptr: custom_pointers)
    {
        tfree(ptr);
    }
    auto end_custom_deallocation = std::chrono::high_resolution_clock::now();
    auto duration_custom_deallocation = std::chrono::duration_cast<std::chrono::microseconds>(
        end_custom_deallocation - start_custom_deallocation).count();
    std::cout << "Custom allocator deallocation time: " << duration_custom_deallocation << " microseconds" << std::endl;
    auto end_custom = std::chrono::high_resolution_clock::now();
    auto duration_custom = std::chrono::duration_cast<std::chrono::microseconds>(end_custom - start_custom).count();
    std::cout << "Custom allocator time (total): " << duration_custom << " microseconds" << std::endl;

    return 0;
}
