#include "../lib/hw.h"
#include "../h/syscall_cpp.hpp"
#include "../h/syscall_c.hpp"
#include "../test/printing.hpp"
#include "../h/bufferControler.hpp"

extern "C" void supervisorTrap();

extern void userMain();

void userWrapper(void*){
    userMain();
}
void waitingPutC(void* arg){
    while(1) {
        noInterrupts();
        while (( (*(uint8 *) (CONSOLE_STATUS)) & 0x20) && BufferControler::outputR()) {
            char c = BufferControler::getOutputBuff();
            *(uint8*)(CONSOLE_TX_DATA) = c;
        }
        allowInterrupts();
        BufferControler::deleteOutput();
        thread_dispatch();
    }
}
thread_t thread = nullptr;
void main() {
    PCB* main1 = PCB::createThread(nullptr, nullptr);
    PCB::running = main1;

    toUser();
    thread_create(&thread, &userWrapper, nullptr);
    thread_t putC;
    thread_create(&putC, &waitingPutC, nullptr);
    BufferControler::createBuffs();
    while(!thread->isFinished()) { thread_dispatch();}
    toSupervisor();
}



