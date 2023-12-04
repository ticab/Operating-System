#ifndef PROJECT_BASE_V1_1_SEMAPHORE_HPP
#define PROJECT_BASE_V1_1_SEMAPHORE_HPP
#include "../h/pcb.hpp"
#include "../h/MemoryAllocator.hpp"

class Sem {
public:
    static Sem *createSem(unsigned init);

    Sem(unsigned short init = 1);
    int wait();
    int signal();
    int value();
    void deleteSem();
private:
    int val = 1;
    Elem *head, *tail;
    PCB *get();
    void put(PCB* pcb);
};


#endif //PROJECT_BASE_V1_1_SEMAPHORE_HPP
