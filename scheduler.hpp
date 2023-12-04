#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_SCHEDULER_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_SCHEDULER_HPP

class PCB;

struct Elem {
    PCB *data;
    Elem *next;
    Elem(PCB *data, Elem *next) : data(data), next(next) {}
};

class Scheduler
{
private:
    static Elem* head, *tail;

public:
    static PCB *get();
    static void put(PCB *pcb);
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_SCHEDULER_HPP
