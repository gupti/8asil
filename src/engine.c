#include "display.h"
#include "charset.h"

typedef struct
{
    /* 4kB of memory */
    unsigned char mem[4096];

    /* General Registers */
    unsigned char reg[16];

    /* 16 bit I register, used to store memory addresses */
    unsigned short i;

    /* Delay and sound timers */
    char delay;
    char sound;

    /* Program Counter */
    unsigned short pc;

    /* Stack Pointer */
    unsigned char sp;
} chipState;

chipState ENGINE_state = {.mem = CHAR_SET};


