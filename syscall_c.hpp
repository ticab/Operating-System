#ifndef PROJECT_BASE_V1_1_SYSCALL_C_HPP
#define PROJECT_BASE_V1_1_SYSCALL_C_HPP
#include "../lib/hw.h"

void* mem_alloc (size_t size);

int mem_free (void* p);

void toUser();

void toSupervisor();

void noInterrupts();

void allowInterrupts();


class PCB;
typedef PCB* thread_t;

int thread_create (PCB** handle, void(*start_routine)(void*), void* arg);

void thread_create2 (PCB** handle, void(*start_routine)(void*), void* arg);

int thread_create1 (PCB** handle);

void thread_dispatch();

int thread_exit();

class Sem;
typedef Sem* sem_t;

int sem_open (sem_t* handle, unsigned init);

int sem_close (sem_t handle);

int sem_wait (sem_t id);

int sem_signal (sem_t id);

typedef unsigned long time_t;

int time_sleep (time_t);

const int EOF = -1;

char getc ();

void putc (char c);


#endif //PROJECT_BASE_V1_1_SYSCALL_C_HPP
