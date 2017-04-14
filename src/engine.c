#include "engine.h"
#include "charset.h"

typedef struct
{
    /* 4kB of memory */
    unsigned char mem[4096];

    /* 16 general registers */
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

    /* Stack size */
    unsigned char ss;
} chipState;

#define addressToShort(x) *((unsigned short *)(&ENGINE_state.mem[(x)]))

static chipState ENGINE_state = {.mem = CHAR_SET};

void ENGINE_loadProgram(char *progName)
{
    ENGINE_reset();
}

void ENGINE_run(char * progName)
{
    ENGINE_reset();
    ENGINE_loadProgram(progName);
}

void ENGINE_reset(void)
{
    /* TODO: Check if mem needs reseting too */
    memset(&ENGINE_state.reg, 0, 16);
    ENGINE_state.i = 0;
    ENGINE_state.delay = 0;
    ENGINE_state.sound = 0;
    ENGINE_state.pc = PROG_START;
    /* Stack starts at memory location 0 */
    ENGINE_state.sp = 0;
    ENGINE_state.ss = 0;
}

static int ENGINE_push(unsigned short address)
{
    return 1;
}

static int ENGINE_pop()
{
    return 1;
}


/* TODO: make sure the Vf register isn't being accessed */
void ENGINE_cycle(void)
{
    /* How much to increment the program counter, 2 (1 instruction) by default */
    int advancePC = 2;

    /* Perhaps add check to see if memory location is even */
    switch(ENGINE_state.mem[ENGINE_state.pc] & 0xf0)
    {
        case 0x00:
            /* Ignore SYS instruction */
            if (ENGINE_state.mem[ENGINE_state.pc] & 0x0f)
            {
                switch(ENGINE_state.mem[ENGINE_state.pc + 1])
                {
                    /* CLS */
                    case 0xe0:
                        DISPLAY_clear();
                        break;
                    /* RET */
                    case 0xee:
                        ENGINE_pop();
                        advancePC = 0;
                        break;
                }
            }
            break;

        /* JP */
        case 0x10:
            ENGINE_state.pc = addressToShort(ENGINE_state.pc) & 0x0fff;
            advancePC = 0;
            break;

        /* CALL */
        case 0x20:
            ENGINE_push(ENGINE_state.pc);
            ENGINE_state.pc = addressToShort(ENGINE_state.pc);
            break;

        /* SE */
        case 0x30:
            if (ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] ==
                ENGINE_state.mem[ENGINE_state.pc + 1])
            {
                advancePC = 2;
            }
            break;

        /* SNE */
        case 0x40:
            if (ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] !=
                ENGINE_state.mem[ENGINE_state.pc + 1])
            {
                advancePC = 2;
            }
            break;

        /* SE */
        case 0x50:
            if ((ENGINE_state.mem[ENGINE_state.pc + 1] & 0x0f) == 0)
            {
                if (ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] ==
                    ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc + 1] & 0xf0])
                {
                    advancePC = 2;
                }
            }
            break;

        /* LD */
        case 0x60:
            ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] = ENGINE_state.mem[ENGINE_state.pc + 1];
            break;

        case 0x70:
            break;

        case 0x80:
            break;

        /* SNE */
        case 0x90:
            if ((ENGINE_state.mem[ENGINE_state.pc + 1] & 0x0f) == 0)
            {
                if (ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] !=
                    ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc + 1] & 0xf0])
                {
                    advancePC = 2;
                }
            }
            break;

    }

    ENGINE_state.pc += advancePC;

}
