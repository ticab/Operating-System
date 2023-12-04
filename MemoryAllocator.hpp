
#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
#include "../lib/hw.h"


class MemoryAllocator {
public:
    struct Mem {
        Mem *next;
        size_t size;  //broj bajtova koje zauzima
    };

    static void getInstance() {
        if(!first) {
            freeMemHead = (Mem*)HEAP_START_ADDR;
            freeMemHead->size = ((((size_t) HEAP_END_ADDR - (size_t) HEAP_START_ADDR - sizeof(Mem)) / MEM_BLOCK_SIZE - 1) * MEM_BLOCK_SIZE);
            freeMemHead->next = nullptr;
            memHead = nullptr;
            first = true;
        }
        return;
    }
    static void *mem_alloc(size_t size);
    static int mem_free(void *ptr);
    static int tryToJoin(Mem* cur);

private:
    MemoryAllocator() {}

    static bool first;
    static Mem *freeMemHead, *memHead;
};



#endif //PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP

