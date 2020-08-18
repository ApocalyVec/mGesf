/**
 * @file
 *
 * @brief Overloaded new and delete operators
 */

#include <cstdlib>
//#include <iostream>
//#include <string>
#include "xt_new_delete.h"
#include "xtmemory.h"
#include "assert.h"

using namespace std;


void* operator new(size_t size)
{
    switch (xtmemory_get_new_memory_type()) {
    case XTMEMORY_DEFAULT:
        return xtmemory_malloc_default(size);
    case XTMEMORY_FAST:
        return xtmemory_malloc_fast(size);
    case XTMEMORY_SLOW:
        return xtmemory_malloc_slow(size);
    default:
        assert(!"corrupt xtmemory_new_memory_type");
    }
}

void* operator new[](size_t size)
{
    return operator new(size);
}

void operator delete(void *p)
{
    if (p == nullptr)
        return;

    switch (xtmemory_classify(p)) {
    case XTMEMORY_DEFAULT:
        return xtmemory_free_default(p);
    case XTMEMORY_FAST:
        return xtmemory_free_fast(p);
    case XTMEMORY_SLOW:
        return xtmemory_free_slow(p);
    default:
        assert(!"free of invalid type");
    }
}

void operator delete[](void *p)
{
    operator delete(p);
}
