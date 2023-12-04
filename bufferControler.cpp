
#include "../h/bufferControler.hpp"
#include "../h/MemoryAllocator.hpp"

int BufferControler::cap = 128;
int BufferControler::head1 = 0, BufferControler::tail1=0, BufferControler::head2=0, BufferControler::tail2 = 0;
char* BufferControler::input = nullptr;
char* BufferControler::output = nullptr;


void BufferControler::putInputBuff(char val){
    input[tail1] = val;
    tail1 = (tail1 + 1) % cap;
}
void BufferControler::putOutputBuff(char val){
    output[tail2] = val;
    tail2 = (tail2 + 1) % cap;
    return;
}
char BufferControler::getInputBuff() {
    char ret = input[head1];
    head1 = (head1 + 1) % cap;
    return ret;
}
char BufferControler::getOutputBuff() {
    char ret = output[head2];
    head2 = (head2 + 1) % cap;
    return ret;
}

void BufferControler::createBuffs() {
    input = (char*) MemoryAllocator::mem_alloc(128);
    output = (char*) MemoryAllocator::mem_alloc(128);
    head1 = 0, head2 = 0;
    tail1 = 0, tail2 = 0;
}

void BufferControler::deleteOutput(){
    head2=0; tail2=0;
}
void BufferControler::deleteInput() {
    head1=0; tail1=0;
}

bool BufferControler::inputR(){
    return tail1-head1!=0;
}
bool BufferControler::outputR(){
    return tail2-head2!=0;
}



