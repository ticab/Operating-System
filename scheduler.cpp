
#include "../h/scheduler.hpp"
#include "../h/MemoryAllocator.hpp"

Elem* Scheduler::head = nullptr;
Elem* Scheduler::tail = nullptr;

PCB *Scheduler::get()
{
    if (!head) return nullptr;
    Elem *elem = head;
    head = head->next;
    if (!head) tail = 0;
    PCB *ret = elem->data;
    MemoryAllocator::mem_free(elem);
    return ret;
}

void Scheduler::put(PCB *pcb) {
    Elem *elem = (Elem *) MemoryAllocator::mem_alloc(sizeof(Elem));
    elem->data = pcb;
    elem->next = 0;
    if (head && tail) {
        tail->next = elem;
        tail = elem;
    } else {
        head = tail = elem;
    }
}
