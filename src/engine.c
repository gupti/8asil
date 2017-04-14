#include "engine.h"
#include "charset.h"
#include <time.h>
#include <stdlib.h>
#include <limits.h>

typedef struct
{
    /* 4kB of memory */
    unsigned char mem[4096];

    /* 16 general registers */
    unsigned char reg[16];

    /* 16 bit I register, used to store memory addresses */
    unsigned short i;

    /* Delay and sound timers */
    char dt;
    char st;

    /* Program Counter */
    unsigned short pc;

    /* Stack Pointer */
    unsigned char sp;

    /* Stack size */
    unsigned char ss;
} chipState;

#define addressTo12bit(x) (*((unsigned short *)(&ENGINE_state.mem[(x)])) & 0x0fff)

static chipState ENGINE_state = {.mem = CHAR_SET};

void ENGINE_loadProgram(char *progName)
{
    ENGINE_reset();
}

void ENGINE_start(char * progName)
{
    ENGINE_reset();
    ENGINE_loadProgram(progName);
}

void ENGINE_run(void)
{
}

void ENGINE_reset(void)
{
    /* TODO: Check if mem needs reseting too */
    memset(&ENGINE_state.reg, 0, 16);
    ENGINE_state.i = 0;
    ENGINE_state.dt = 0;
    ENGINE_state.st = 0;
    ENGINE_state.pc = PROG_START;
    /* Stack starts at memory location 0 */
    ENGINE_state.sp = 0;
    ENGINE_state.ss = 0;
    srand(time(NULL));
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
    /* How much to increment the program counter, 2 bytes (1 instruction) by default */
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
            ENGINE_state.pc = addressTo12bit(ENGINE_state.pc);
            advancePC = 0;
            break;

        /* CALL */
        case 0x20:
            ENGINE_push(ENGINE_state.pc);
            ENGINE_state.pc = addressTo12bit(ENGINE_state.pc);
            break;

        /* SE */
        case 0x30:
            if (ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] ==
                ENGINE_state.mem[ENGINE_state.pc + 1])
            {
                advancePC = 4;
            }
            break;

        /* SNE */
        case 0x40:
            if (ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] !=
                ENGINE_state.mem[ENGINE_state.pc + 1])
            {
                advancePC = 4;
            }
            break;

        /* SE */
        case 0x50:
            if ((ENGINE_state.mem[ENGINE_state.pc + 1] & 0x0f) == 0)
            {
                if (ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] ==
                    ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc + 1] & 0xf0])
                {
                    advancePC = 4;
                }
            }
            break;

        /* LD */
        case 0x60:
            ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] = ENGINE_state.mem[ENGINE_state.pc + 1];
            break;

        /* ADD */
        case 0x70:
            ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] += ENGINE_state.mem[ENGINE_state.pc + 1];
            break;

        case 0x80:
            switch(ENGINE_state.mem[ENGINE_state.pc + 1] & 0x0f)
            {
                /* LD */
                case 0x00:
                    ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] = ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc + 1] & 0xf0];
                    break;

                /* OR */
                case 0x01:
                    ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] |= ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc + 1] & 0xf0];
                    break;

                /* AND */
                case 0x02:
                    break;
                    ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] &= ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc + 1] & 0xf0];

                /* XOR */
                case 0x03:
                    ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] ^= ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc + 1] & 0xf0];
                    break;

                /* ADD */
                case 0x04:
                    {
                        short sum = (short)ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] + ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc + 1] & 0xf0];
                        if (sum > UCHAR_MAX)
                        {
                            ENGINE_state.reg[0xf] = 1;
                        }
                        else
                        {
                            ENGINE_state.reg[0xf] = 0;
                        }
                        ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] = sum;
                    }
                    break;

                /* SUB */
                case 0x05:
                    if (ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0xf0] > ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc + 1] & 0xf0])
                    {
                        ENGINE_state.reg[0xf] = 1;
                    }
                    else
                    {
                       ENGINE_state.reg[0xf] = 0;
                    }
                    ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] -= ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc + 1] & 0xf0];
                    break;

                /* SHR */
                case 0x06:
                    if (ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0xf0] & 0x1)
                    {
                        ENGINE_state.reg[0xf] = 1;
                    }
                    else
                    {
                       ENGINE_state.reg[0xf] = 0;
                    }
                    ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] >>= 1;
                    break;

                /* SUBN */
                case 0x07:
                    if (ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0xf0] < ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc + 1] & 0xf0])
                    {
                        ENGINE_state.reg[0xf] = 1;
                    }
                    else
                    {
                       ENGINE_state.reg[0xf] = 0;
                    }
                    ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] = ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] -
                        ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc + 1] & 0xf0];
                    break;

                /* SHL  */
                case 0x0e:
                    if (ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0xf0] & 0x8)
                    {
                        ENGINE_state.reg[0xf] = 1;
                    }
                    else
                    {
                       ENGINE_state.reg[0xf] = 0;
                    }
                    ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] <<= 1;
                    break;
            }
            break;

        /* SNE */
        case 0x90:
            if ((ENGINE_state.mem[ENGINE_state.pc + 1] & 0x0f) == 0)
            {
                if (ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] !=
                    ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc + 1] & 0xf0])
                {
                    advancePC = 4;
                }
            }
            break;

        /* LD */
        case 0xa0:
            ENGINE_state.i = addressTo12bit(ENGINE_state.mem[ENGINE_state.pc]);
            break;

        /* JP */
        case 0xb0:
            ENGINE_state.pc = ENGINE_state.reg[0x0] + addressTo12bit(ENGINE_state.mem[ENGINE_state.pc]);
            break;

        /* RND */
        case 0xc0:
            ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] = (rand() % (UCHAR_MAX + 1)) & ENGINE_state.mem[ENGINE_state.pc + 1];
            break;

        /* DRW */
        case 0xd0:
            DISPLAY_drawSprite(ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f],
                    ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc + 1] & 0xf0],
                    &ENGINE_state.mem[ENGINE_state.i],
                    ENGINE_state.mem[ENGINE_state.pc + 1] & 0x0f);
            break;

        /* TODO: Stubs, no input yet */
        case 0xe0:
            switch(ENGINE_state.mem[ENGINE_state.pc + 1])
            {
                case 0x9e:
                    break;
                case 0xa1:
                    break;
            }
            break;

        case 0xf0:
            switch(ENGINE_state.mem[ENGINE_state.pc + 1])
            {
                case 0x07:
                    break;
                case 0x0a:
                    break;
                case 0x15:
                    break;
                case 0x18:
                    break;

                /* ADD */
                case 0x1e:
                    ENGINE_state.i += ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc & 0x0f]];
                    break;

                /* LD */
                case 0x29:
                    ENGINE_state.i = CHAR_START + ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f] * CHAR_HEIGHT;
                    break;

                /* LD */
                case 0x33:
                    {
                        unsigned char regValue = ENGINE_state.reg[ENGINE_state.mem[ENGINE_state.pc] & 0x0f];
                        ENGINE_state.mem[ENGINE_state.i] = regValue / 100;
                        ENGINE_state.mem[ENGINE_state.i + 1] = regValue / 10 - ENGINE_state.mem[ENGINE_state.i];
                        ENGINE_state.mem[ENGINE_state.i + 2] = regValue - ENGINE_state.mem[ENGINE_state.i] - ENGINE_state.mem[ENGINE_state.i + 1];
                    }
                    break;

                /* LD */
                case 0x55:
                    memcpy(&ENGINE_state.mem[ENGINE_state.i], &ENGINE_state.reg, 16);
                    break;

                /* LD */
                case 0x65:
                    memcpy(&ENGINE_state.reg, &ENGINE_state.mem[ENGINE_state.i], 16);
                    break;
            }
            break;

    }

    ENGINE_state.pc += advancePC;
    if(ENGINE_state.pc > 4095)
    {
        // error!
    }
}
