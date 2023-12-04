
#ifndef _syscall_cpp
#define _syscall_cpp
#include "../lib/hw.h"
#include "../h/pcb.hpp"

class Sem;


class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t);
protected:
    Thread ();
    virtual void run () {}
    friend class PCB;
    friend void wrapper(void* thread);
private:
    PCB* myPCB=nullptr;
};

class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore();
    int wait ();
    int signal ();
private:
    Sem* myHandle;
};
class Console {
public:
    static char getc ();
    static void putc (char);
};

class PeriodicThread : public Thread {
protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation () {}
};


#endif //_syscall_cpp
