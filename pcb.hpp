
#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_1_SYNCHRONOUS_CCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_1_SYNCHRONOUS_CCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/riscv.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../test/printing.hpp"
#include "../h/scheduler.hpp"

class BufferControler;

extern "C" void handleSupervisorTrap();
class Thread;

// Process Control Block

class PCB
{
public:
    ~PCB() { MemoryAllocator::mem_free(stack); }

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    using Body = void (*)(void*);

    static PCB *createThread(Body body, void* arg);

    int proveri();

    bool hasStarted() const { return started; }

    void setStarted() { started = true; }

    int putInScheduler();

    static void yield();

    static PCB *running;

    static Elem* head;

    static int putSleepy(time_t t);

    static void removeSleepy(time_t);

    uint64 getTimeSlice() const { return timeSlice; }

private:
    void* operator new(size_t n);
    void* operator new[](size_t n);
    PCB(Body body) :
            body(body),
            stack(body != nullptr ? new uint64[DEFAULT_STACK_SIZE] : nullptr),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0
                    }),
            finished(false),
            started(false)
    {
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    uint64 *stack;
    Context context;
    bool finished;
    bool started;
    void* arg = nullptr;
    bool deallocated = false;
    uint64 timeSlice;
    static uint64 timeSliceCounter;

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();
    static void threadWrapper();
    friend class Semaphore;
    friend class Thread;
    friend class Riscv;
    friend void handleSupervisorTrap();
    friend class Sem;
    friend class BufferControler;

    void addArg(void* arg);

    time_t sleepTime;

    static uint64 constexpr TIME_SLICE = DEFAULT_TIME_SLICE;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_1_SYNCHRONOUS_CCB_HPP
