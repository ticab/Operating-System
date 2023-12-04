
#ifndef PROJEKAT_BUFFERCONTROLER_HPP
#define PROJEKAT_BUFFERCONTROLER_HPP
#include "../h/pcb.hpp"

extern "C" void handleSupervisorTrap();
class BufferControler {
public:
    static bool inputR();
    static bool outputR();
    static void createBuffs();
    static void deleteOutput();
    static void deleteInput();

    static void putInputBuff(char val);
    static void putOutputBuff(char val);
    static char getInputBuff();
    static char getOutputBuff();

private:
    static int cap;
    static int head1, tail1, head2, tail2;
    static char* input;
    static char* output;
    friend void handleSupervisorTrap();
};


#endif //PROJEKAT_BUFFERCONTROLER_HPP
