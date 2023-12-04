#include "../h/MemoryAllocator.hpp"

MemoryAllocator::Mem* MemoryAllocator::freeMemHead = (MemoryAllocator::Mem*) HEAP_START_ADDR;
MemoryAllocator::Mem* MemoryAllocator::memHead = nullptr;
bool MemoryAllocator::first = false;

int MemoryAllocator::tryToJoin (Mem* cur) {
    if (!cur) return 0;

    if (cur->next && (char*)cur+cur->size == (char*)(cur->next)) {
        cur->size += cur->next->size;
        cur->next = cur->next->next;
        return 1;
    } else
        return 0;
}

int MemoryAllocator::mem_free(void *ptr) {
    getInstance();
    Mem* cur, *blk = (Mem*) ((char*)ptr-sizeof(Mem));
    if(!memHead->next) memHead = nullptr;
    for(Mem* x = memHead; x!=nullptr && x->next!=nullptr; x=x->next) {
        if (x->next == blk) {
            x->next = blk->next;
            break;
        }
    }
    if (!freeMemHead || blk<freeMemHead)
        cur = 0;
    else
        for (cur=freeMemHead; cur->next!=0 && blk>cur->next; cur=cur->next);
    Mem* newSeg = blk;
    if (cur) newSeg->next = cur->next;
    else newSeg->next = freeMemHead;
    if (cur) cur->next = newSeg;
    else freeMemHead = newSeg;
    tryToJoin(newSeg);
    tryToJoin(cur);

    return 0;
}


void *MemoryAllocator::mem_alloc(size_t size) {
    getInstance();
    Mem *blk = freeMemHead, *prev = nullptr;
    for (; blk!=nullptr; prev=blk, blk=blk->next)
        if (blk->size>=size) break;
    if (blk == nullptr) {
        return nullptr;
    }
    size_t remainingSize = blk->size - size;
    if (remainingSize >= sizeof(Mem) + MEM_BLOCK_SIZE) {
        blk->size = size+sizeof(Mem);
        Mem* newBlk = (Mem*)((char*)blk + blk->size);
        if (prev) prev->next = newBlk;
        else freeMemHead = newBlk;
        newBlk->next = blk->next;
        newBlk->size = remainingSize - sizeof(Mem);
        Mem* cur;
        if (!memHead || blk<memHead)
            cur = nullptr;
        else
            for (cur=memHead; cur->next!=0 && blk>cur->next; cur=cur->next);
        Mem* newSeg = blk;
        if (cur) newSeg->next = cur->next;
        else newSeg->next = memHead;
        if (cur) cur->next = newSeg;
        else memHead = newSeg;
    } else {
        if (prev) prev->next = blk->next;
        else freeMemHead = blk->next;
        Mem* cur;
        if (!memHead || blk<memHead)
            cur = nullptr;
        else
            for (cur=memHead; cur->next!=0 && blk>cur->next; cur=cur->next);
        Mem* newSeg = blk;
        if (cur) newSeg->next = cur->next;
        else newSeg->next = memHead;
        if (cur) cur->next = newSeg;
        else memHead = newSeg;
    }
    blk->next = nullptr;
    return (char*)blk + sizeof(Mem);

}