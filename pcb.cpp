
#include "../h/pcb.hpp"
#include "../h/MemoryAllocator.hpp"
uint64 PCB::timeSliceCounter = 0;

Elem *PCB::head = nullptr;

PCB *PCB::running = nullptr;
void* PCB::operator new(size_t n) {
    return MemoryAllocator::mem_alloc(n);
}

void *PCB::operator new[](size_t n) {
    return MemoryAllocator::mem_alloc(n);
}

PCB *PCB::createThread(PCB::Body body, void *arg) {
    PCB *novi = ((PCB*) MemoryAllocator::mem_alloc(sizeof(PCB)));
    novi->body = body;
    if(body!=nullptr) novi->stack = (uint64*)( MemoryAllocator::mem_alloc((sizeof(uint64))*DEFAULT_STACK_SIZE));
    else novi->stack = nullptr;
    novi->context = Context({(uint64)&(novi->threadWrapper), novi->stack != nullptr ? (uint64) &novi->stack[DEFAULT_STACK_SIZE] : 0});
    novi->finished=false;
    novi->started=false;
    novi->addArg(arg);
    novi->deallocated = false;
    novi->timeSlice = TIME_SLICE;
    novi->sleepTime = 0;
    return novi;

}

void PCB::addArg(void* arg){
    this->arg = arg;
}
int PCB::putInScheduler(){
    if (body != nullptr) {
        Scheduler::put(this);
        return 0;
    }
    return -1;
}

void PCB::yield()
{
    __asm__ volatile("mv a0, %0"::"r"(0x13));
    __asm__ volatile ("ecall");
}

void PCB::dispatch()
{
    PCB *old = running;
    if (!old->isFinished()) {
        __asm__ volatile("mv s1,%0"::"r"(old));
        Scheduler::put(old);
    }
    running = Scheduler::get();

    PCB::contextSwitch(&old->context, &running->context);
}
void PCB::threadWrapper() {
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    PCB::yield();
}

int PCB::proveri() {
    if(this->body) return 0;
    else return -1;
}

int PCB::putSleepy(time_t t) {
    PCB* old = PCB::running;
    old->sleepTime = t;


    Elem *prev = nullptr, *cur = head;
    for(; cur!= nullptr; cur=cur->next){
        if(cur->data->sleepTime > old->sleepTime) break;
        prev = cur;
    }
    Elem *elem = (Elem*) MemoryAllocator::mem_alloc(sizeof(Elem));
    if(elem == nullptr) return -1;
    elem->data = old;
    elem->next = cur;
    if(prev) prev->next = elem;
    else head = elem;


    running = Scheduler::get();
    if(running == nullptr) return -1;
    contextSwitch(&old->context, &running->context);
    return 0;
}

void PCB::removeSleepy(time_t t) {
    while(head!=nullptr){
        if(head->data->sleepTime == t) {
            head->data->putInScheduler();
            Elem* old = head;
            head=head->next;
            MemoryAllocator::mem_free(old);
            old->data = nullptr; old->next = nullptr; old = nullptr;
        }
        else break;
    }
}
