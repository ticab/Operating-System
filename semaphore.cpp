#include "../h/semaphore.hpp"

Sem *Sem::createSem(unsigned int init) {
    Sem* sem = (Sem*)MemoryAllocator::mem_alloc(sizeof(Sem));
    sem->val = init;
    sem->head = nullptr;
    sem->tail = nullptr;
   // sem->blocked = (Queue*)MemoryAllocator::mem_alloc(sizeof(Queue));
   //sem->blocked->head = nullptr;
   // sem->blocked->tail = nullptr;
    return sem;
}
Sem::Sem(unsigned short init) {
    val=init;
}
int Sem::value() { return val; }
int Sem::wait() {
    if(--val<0){
        PCB* old = PCB::running;
        if(old->deallocated) return -1;
        put(old);
        PCB::running = Scheduler::get();
        PCB* running = PCB::running;
        PCB::contextSwitch(&old->context, &running->context);
    }
    return 0;
}

int Sem::signal() {
    if(val++<0) {
        Scheduler::put(get());
    }
    return 0;
}

void Sem::deleteSem() {
    PCB* bl;
    while((bl=get())!=nullptr){
        bl->deallocated=true;
        Scheduler::put(bl);
    }
}

PCB *Sem::get() {
    if(!head) return nullptr;
    Elem *elem = head;
    head = head->next;
    if (!head) tail = 0;

    PCB *ret = elem->data;
    MemoryAllocator::mem_free(elem);
    elem->data = nullptr; elem->next = nullptr; elem = nullptr;
    return ret;
}

void Sem::put(PCB *pcb) {
    Elem *elem = (Elem*)MemoryAllocator::mem_alloc(sizeof(Elem));
    elem->data = pcb;
    elem->next = 0;
    if (head && tail) {
        tail->next = elem;
        tail = elem;
    } else {
        head = tail = elem;
    }
}
