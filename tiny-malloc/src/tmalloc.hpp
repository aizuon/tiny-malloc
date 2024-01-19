#ifndef TMALLOC_H
#define TMALLOC_H

#include <cstddef>

void* tmalloc(std::size_t size);

void tfree(void* ptr);

#endif //TMALLOC_H
