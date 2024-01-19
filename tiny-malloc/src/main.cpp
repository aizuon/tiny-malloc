#include "tmalloc.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    void* ptr = tmalloc(1000);
    std::cout << "ptr: " << ptr << std::endl;
    tfree(ptr);

    return 0;
}
