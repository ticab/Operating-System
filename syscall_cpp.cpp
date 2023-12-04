
#include "../h/syscall_cpp.hpp"
#include "../h/syscall_c.hpp"
#include "../lib/hw.h"
#include "../h/bufferControler.hpp"


Thread::Thread(void (*body)(void*), void* arg) {
    thread_create(&myPCB, body, arg);
}

Thread::~Thread() {
    thread_exit();
}

void wrapper(void* thread) {
    if (thread)
        ((Thread *) thread)->run();
}

int Thread::start() {
    if(myPCB->hasStarted()) return 0;
    else{
        myPCB->setStarted();
        return thread_create1(&myPCB);
    }
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t t) { return t==0 ? 0 : time_sleep(t); }

Thread::Thread() {
    thread_create2(&myPCB, wrapper, this);
}

Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}

PeriodicThread::PeriodicThread(time_t period) {
    periodicActivation();
    sleep(period);
}
