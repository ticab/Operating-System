
#include "../h/syscall_c.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/riscv.hpp"
#include "../h/pcb.hpp"
#include "../h/semaphore.hpp"
#include "../h/bufferControler.hpp"

extern "C" void supervisorTrap();
extern thread_t thread;
class Thread;

void* mem_alloc (size_t size){
    __asm__ volatile("mv a1, %0" : : "r"(size));
    __asm__ volatile("mv a0, %0": :"r" (0x01)); //kod sistemskog poziva mem_alloc je 0x01

    __asm__ volatile("ecall");
    void* vr;
    __asm__ volatile("mv %0, a0":"=r"(vr));
    return vr;
}

int mem_free (void* p){
    size_t pp = (size_t) p;
    __asm__ volatile("mv a1, %0" : : "r"(pp));
    __asm__ volatile("mv a0, %0": :"r" (0x02)); //kod sistemskog poziva mem_free je 0x02
    __asm__ volatile("ecall");

    int vr;
    __asm__ volatile("mv %0, a0":"=r"(vr));
    return vr;
}

void toUser(){
    __asm__ volatile("mv a0, %0": :"r" (0x03));
    __asm__ volatile("csrw stvec, %0"::"r"(&supervisorTrap));
    __asm__ volatile("ecall");
    return;
}
void toSupervisor(){
    __asm__ volatile("mv a0, %0": :"r" (0x04));
    __asm__ volatile("ecall");
    return;
}
void noInterrupts(){
    __asm__ volatile("mv a0, %0"::"r"(0x05));
    __asm__ volatile("ecall");
    return;
}
void allowInterrupts(){
    __asm__ volatile("mv a0, %0"::"r"(0x06));
    __asm__ volatile("ecall");
    return;
}
int thread_create (PCB** handle, void(*start_routine)(void*), void* arg){ //kreira nit i stavlja u scheduler
    __asm__ volatile("mv a3, %0" : : "r"(arg));
    __asm__ volatile("mv a2, %0" : : "r"((start_routine)));
    __asm__ volatile("mv a1, %0" : : "r"((uint64)handle));


    __asm__ volatile("mv a0, %0": :"r" (0x11));
    __asm__ volatile("ecall");
    return 0;
}
int thread_create1 (PCB** handle){ //samo stavlja u scheduler
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("mv a0, %0": :"r" (0x9)); //kod sistemskog poziva thread_create1 je 0x11
    __asm__ volatile("ecall");
    int vr;
    __asm__ volatile("mv %0, a1":"=r"(vr));
    //__asm__ volatile("csrw sstatus, zero");
    return vr;
}
void thread_create2 (PCB** handle, void(*start_routine)(void*), void* arg){//samo kreira nit
    __asm__ volatile("mv a4, s1");
    __asm__ volatile("mv a3, %0" : : "r"(arg));
    __asm__ volatile("mv a2, %0" : : "r"((start_routine)));
    __asm__ volatile("mv a1, %0" : : "r"((uint64)handle));


    __asm__ volatile("mv a0, %0": :"r" (0x10)); //kod sistemskog poziva thread_create2 je 0x10
    __asm__ volatile("ecall");
   // __asm__ volatile("csrw sstatus, zero");
}
int thread_exit(){
    __asm__ volatile("mv a0, %0":: "r"(0x12));
    __asm__ volatile("ecall");
    int vr;
    __asm__ volatile("mv %0, a0":"=r"(vr));
    return vr;
}

void thread_dispatch(){
    __asm__ volatile("mv a0, %0": : "r"(0x13));
    __asm__ volatile("ecall");
    return;
}

int sem_open (Sem** handle, unsigned init){
    __asm__ volatile("mv a2, %0" : : "r"(init));
    __asm__ volatile("mv a1, %0" : : "r"((uint64)handle));
    __asm__ volatile("mv a0, %0": :"r" (0x21));
    __asm__ volatile("ecall");
    int vr;
    __asm__ volatile("mv %0, a1":"=r"(vr));
    return vr==0 ? 0 : -1;
}

int sem_close (sem_t handle){
    __asm__ volatile("mv a1, %0": : "r"(handle));
    __asm__ volatile("mv a0, %0": :"r" (0x22));
    __asm__ volatile("ecall");
    int vr;
    __asm__ volatile("mv %0, a1":"=r"(vr));
    return vr==0 ? 0 : -1;
}

int sem_wait (sem_t id){
    __asm__ volatile("mv a1, %0": : "r"(id));
    __asm__ volatile("mv a0, %0": :"r" (0x23));
    __asm__ volatile("ecall");
    int vr;
    __asm__ volatile("mv %0, a1":"=r"(vr));
    return vr==0 ? 0 : -1;
}

int sem_signal (sem_t id){
    __asm__ volatile("mv a1, %0": : "r"(id));
    __asm__ volatile("mv a0, %0": : "r"(0x24));
    __asm__ volatile("ecall");
    int vr;
    __asm__ volatile("mv %0, a1":"=r"(vr));
    return vr==0 ? 0 : -1;
}

int time_sleep(time_t t){
    __asm__ volatile("mv a1, %0": :"r"(t));
    __asm__ volatile("mv a0, %0": :"r"(0x31));
    __asm__ volatile("ecall");
    int vr;
    __asm__ volatile("mv %0, a1":"=r"(vr));
    return vr==0 ? 0 : -1;
}

char getc(){
    __asm__ volatile("mv a0, %0": :"r" (0x41));
    __asm__ volatile("ecall");
    int vr;
    __asm__ volatile("mv %0, a0":"=r"(vr));
    return vr;
}
void putc (char c){
    __asm__ volatile("mv a1, %0" : : "r"(c));
    __asm__ volatile("mv a0, %0": :"r" (0x42));
    __asm__ volatile("ecall");
    return;
}

time_t globalTime = 0;

extern "C" void handleSupervisorTrap(){
    uint64 volatile scause;
    __asm__ volatile ("csrr %0, scause" : "=r"(scause));
    uint64 volatile sepc;
    __asm__ volatile ("csrr %0, sepc" : "=r"(sepc));
    sepc=sepc+4;
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %0, sstatus":"=r"(sstatus));
    uint64 volatile a0, a1, a2, a3;
    __asm__ volatile("mv %0, a0":"=r"(a0));
    __asm__ volatile("mv %0, a1":"=r"(a1));
    __asm__ volatile("mv %0, a2":"=r"(a2));
    __asm__ volatile("mv %0, a3":"=r"(a3));

    if(scause ==  0x8000000000000001){
        Riscv::mc_sip(Riscv::SIP_SSIP);
        PCB::timeSliceCounter++;
        globalTime++;
        if(PCB::head != nullptr && globalTime==(PCB::head)->data->sleepTime){
            PCB::removeSleepy(globalTime);
        }
        if (PCB::timeSliceCounter >= PCB::running->getTimeSlice()){
            uint64 volatile sepc = Riscv::r_sepc();
            uint64 volatile sstatus = Riscv::r_sstatus();
            PCB::timeSliceCounter = 0;
            PCB::dispatch();
            Riscv::w_sstatus(sstatus);
            Riscv::w_sepc(sepc);
        }
    }
    else if(scause == 0x0000000000000009){ //toUser
        if(a0==3){
            __asm__ volatile ("csrw sip, zero");
            __asm__ volatile("csrw sstatus, zero");
            Riscv::w_sepc(sepc);
            return;
        }
    }
    else if(scause == ((0x0UL<<63) | 0x08) || scause == 0x0000000000000009){ //ecall iz sistemskog rezima
        PCB::timeSliceCounter = 0;

        switch(a0){
            case 0x1: {
                if(a1%MEM_BLOCK_SIZE!=0) a1=(a1/MEM_BLOCK_SIZE)*MEM_BLOCK_SIZE + MEM_BLOCK_SIZE;
                MemoryAllocator::mem_alloc(a1);
                break;
            }
            case 0x2: {
                MemoryAllocator::mem_free((void*)a1);
                break;
            }
            case 0x4: {
                __asm__ volatile("mv t0, %0"::"r"(0x102));
                __asm__ volatile("csrw sstatus, t0");
                __asm__ volatile("csrw sie, zero");
                Riscv::w_sepc(sepc);
                return;
            }
            case 0x5: //noInterrupts
                __asm__ volatile("csrw sie, zero");
                break;
            case 0x6://allowInterrupts
                __asm__ volatile("mv t0, %0"::"r"(0x222));
                __asm__ volatile("csrw sie, t0");
                break;
            case 0x9:{//thread_create1
                PCB** handle = (PCB**) a1;
                (*handle)->putInScheduler();
                int rez = (*handle)->proveri();
                __asm__ volatile("mv a1, %0": : "r"(rez));
                __asm__ volatile("mv a0, %0": : "r"(handle));
                break;
            }
            case 0x10: { //thread_create2
                PCB**handle = (PCB**) a1;
                void (*start_routine)(void *);
                start_routine = (void (*)(void*)) a2;
                void *arg = (void*) a3;
                *handle=PCB::createThread(start_routine, arg);
                __asm__ volatile("mv a0, %0": : "r"(handle));
                break;
            }
            case 0x11: {//thread_create
                PCB**handle = (PCB**) a1;
                void (*start_routine)(void *);
                start_routine = (void (*)(void*)) a2;
                void *arg = (void*) a3;
                *handle=PCB::createThread(start_routine, arg);
                (*handle)->putInScheduler();
                __asm__ volatile("mv a0, %0": : "r"(handle));
                break;
            }
            case 0x12:{//thread_exit
                PCB::running->setFinished(true);
                PCB::dispatch();
                break;
            }
            case 0x13:{ //thread_dispatch
                PCB::dispatch();
                break;
            }
            case 0x21:{ //sem_open
                Sem** handle = (Sem**) a1;
                unsigned init = a2;
                (*handle) = Sem::createSem(init);
                //if((*handle)==nullptr)  __asm__ volatile("mv a1, %0": :"r"(0x1));
                __asm__ volatile("mv a1, %0"::"r"(0));
                __asm__ volatile("mv a0, %0": :"r"(handle));
                break;
            }
            case 0x22:{ //sem_close
                sem_t handle = (sem_t) a1;
                handle->deleteSem();
                __asm__ volatile("mv a1, a0");
                break;
            }
            case 0x23:{//sem_wait
                sem_t handle = (sem_t) a1;
                int wait = 0;
                if(handle != nullptr)  wait = handle->wait();
                __asm__ volatile("mv a1, %0"::"r"(wait));
                break;
            }
            case 0x24:{
                sem_t handle = (sem_t) a1;
                int signal = 0;
                if(handle!=nullptr) signal = handle->signal();
                __asm__ volatile("mv a1, %0"::"r"(signal));
                break;
            }
            case 0x31:{ //time_sleep
                time_t t = (time_t) a1;
                int a = PCB::putSleepy(globalTime+t);
                __asm__ volatile("mv a1, %0"::"r"(a));
                break;
            }
            case 0x41:{//getc
                while(1){
                    if(!BufferControler::inputR()) {
                        PCB::putSleepy((uint64)1+globalTime);
                    }
                    else break;
                }
                char c = BufferControler::getInputBuff();
                __asm__ volatile("mv %0, a0":"=r"(c));

                break;
            }
            case 0x42:{//putc
                char c = (char)a1;
                BufferControler::putOutputBuff(c);
                //PCB::dispatch();
                break;
            }
        }
        Riscv::w_sstatus(sstatus);
        Riscv::w_sepc(sepc);
    }
    else if(scause == 0x8000000000000009){ //spoljasnji hardverski prekid
        int a = plic_claim();
        if(a==10){
            while ((*(uint8 *) (CONSOLE_STATUS)) & 1){
                char k = (char)(*(uint8*)CONSOLE_RX_DATA);
                BufferControler::putInputBuff(k);
                plic_complete(a);
            }
        }
        plic_complete(a);
    }
    else { //greska
        thread->setFinished(true);
        __asm__ volatile("csrw sip, zero");
        Riscv::w_sstatus(sstatus);
        Riscv::w_sepc(sepc);
    }
}