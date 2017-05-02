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
    char dt;
    char st;

    /* Program Counter */
    unsigned short pc;

    /* Stack Pointer */
    unsigned char sp;

    /* Stack size */
    unsigned char ss;
} chipState;

/* Macros for the current instruction and */
//#define INS1                ENGINE_state.mem[ENGINE_state.pc]
//#define INS2                ENGINE_state.mem[ENGINE_state.pc + 1]
//#define addressTo12bit(x)   (((unsigned short)(ENGINE_state.mem[(x)] & 0x0f) << 8) + ENGINE_state.mem[(x) + 1])

static chipState ENGINE_state = {.mem = CHAR_SET};

unsigned short addressTo12bit(unsigned short x)
{
    unsigned short num = (unsigned short)(ENGINE_state.mem[(x)] & 0x0f);
    num <<= 8;
    num += ENGINE_state.mem[(x) + 1];
    return num;
}

char ENGINE_loadProgram(char *progName)
{
    ENGINE_reset();
    return SYS_loadProgram(progName, &ENGINE_state.mem[PROG_START], 4096 - PROG_START);
}

void ENGINE_start(char * progName)
{
    if(!ENGINE_loadProgram(progName))
    {
        if (DISPLAY_init(CHIP, 8))
        {
            DISPLAY_clear();
            ENGINE_run();
        }
    }
}

void ENGINE_reset(void)
{
    /* TODO: Check if mem needs reseting too */
    memset(&ENGINE_state.reg, 0, 16);
    ENGINE_state.i = 0;
    ENGINE_state.dt = 0;
    ENGINE_state.st = 0;
    /* Stack starts at memory location 0 */
    ENGINE_state.sp = 0;
    ENGINE_state.ss = 0;
    ENGINE_state.pc = PROG_START;
    SYS_init();
}

/* TODO: ENGINE_push and ENGINE_pop are currently just poor hacks so testing can begin */
static char ENGINE_push(void)
{
    if (sizeof(short) * MAX_STACK <= ENGINE_state.sp)
    {
        printf("No more stack space!\n");
        return 2;
    }
    ENGINE_state.sp += 2;
    memcpy(&ENGINE_state.mem[ENGINE_state.sp], &ENGINE_state.pc, 2);
    printf("stackpointer = %u, new address = %u\n", ENGINE_state.sp, addressTo12bit(ENGINE_state.pc));
    ENGINE_state.pc = addressTo12bit(ENGINE_state.pc);
    return 0;
}

static char ENGINE_pop(void)
{
    if (ENGINE_state.sp == 0)
    {
        printf("Nothing to pop!");
        return 2;
    }
    ENGINE_state.pc = addressTo12bit(ENGINE_state.mem[ENGINE_state.sp]);
    ENGINE_state.sp -= 2;
    printf("Return address = %u, stackpointer = %u\n", ENGINE_state.pc, ENGINE_state.sp);
    return 0;
}


/* TODO: make sure the Vf register isn't being accessed */
void ENGINE_cycle(void)
{
    /* How much to increment the program counter, 2 bytes (1 instruction) by default */
    char advancePC = 2;

    unsigned char INS1 = ENGINE_state.mem[ENGINE_state.pc];
    unsigned char INS2 = ENGINE_state.mem[ENGINE_state.pc + 1];
    
//    printf("Current address = %u, Instruction1 = %#04x, Instruction2 = %#04x\n", ENGINE_state.pc, INS1, INS2);
    printf("PC: %d, INS1: %#04x, INS2: %#04x\n", ENGINE_state.pc, INS1, INS2);
    /* Perhaps add check to see if memory location is even */
    switch(INS1 & 0xf0)
    {
        case 0x00:
            /* Ignore SYS instruction */
            if (INS1 & 0x0f)
            {
                switch(INS2)
                {
                    /* CLS */
                    case 0xe0:
                        DISPLAY_clear();
                        break;

                    /* RET */
                    case 0xee:
                        advancePC = ENGINE_pop();
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
            advancePC = ENGINE_push();
            break;

        /* SE */
        case 0x30:
            if (ENGINE_state.reg[INS1 & 0x0f] ==
                INS2)
            {
                advancePC = 4;
            }
            break;

        /* SNE */
        case 0x40:
            if (ENGINE_state.reg[INS1 & 0x0f] !=
                INS2)
            {
                advancePC = 4;
            }
            break;

        /* SE */
        case 0x50:
            if ((INS2 & 0x0f) == 0)
            {
                if (ENGINE_state.reg[INS1 & 0x0f] ==
                    ENGINE_state.reg[(INS2 & 0xf0) >> 4])
                {
                    advancePC = 4;
                }
            }
            break;

        /* LD */
        case 0x60:
            ENGINE_state.reg[INS1 & 0x0f] = INS2;
            break;

        /* ADD */
        case 0x70:
            ENGINE_state.reg[INS1 & 0x0f] += INS2;
            break;

        case 0x80:
            switch(INS2 & 0x0f)
            {
                /* LD */
                case 0x00:
                    ENGINE_state.reg[INS1 & 0x0f] = ENGINE_state.reg[(INS2 & 0xf0) >> 4];
                    break;

                /* OR */
                case 0x01:
                    ENGINE_state.reg[INS1 & 0x0f] |= ENGINE_state.reg[(INS2 & 0xf0) >> 4];
                    break;

                /* AND */
                case 0x02:
                    ENGINE_state.reg[INS1 & 0x0f] &= ENGINE_state.reg[(INS2 & 0xf0) >> 4];
                    break;

                /* XOR */
                case 0x03:
                    ENGINE_state.reg[INS1 & 0x0f] ^= ENGINE_state.reg[(INS2 & 0xf0) >> 4];
                    break;

                /* ADD */
                case 0x04:
                    {
                        short sum = (short)ENGINE_state.reg[INS1 & 0x0f] + ENGINE_state.reg[(INS2 & 0xf0) >> 4];
                        if (sum > UCHAR_MAX)
                        {
                            ENGINE_state.reg[0xf] = 1;
                        }
                        else
                        {
                            ENGINE_state.reg[0xf] = 0;
                        }
                        ENGINE_state.reg[INS1 & 0x0f] = sum;
                    }
                    break;

                /* SUB */
                case 0x05:
                    if (ENGINE_state.reg[INS1 & 0x0f] > ENGINE_state.reg[(INS2 & 0xf0) >> 4])
                    {
                        ENGINE_state.reg[0xf] = 1;
                    }
                    else
                    {
                       ENGINE_state.reg[0xf] = 0;
                    }
                    ENGINE_state.reg[INS1 & 0x0f] -= ENGINE_state.reg[(INS2 & 0xf0) >> 4];
                    break;

                /* SHR */
                case 0x06:
                    if (ENGINE_state.reg[INS1 & 0x0f] & 0x1)
                    {
                        ENGINE_state.reg[0xf] = 1;
                    }
                    else
                    {
                       ENGINE_state.reg[0xf] = 0;
                    }
                    ENGINE_state.reg[INS1 & 0x0f] >>= 1;
                    break;

                /* SUBN */
                case 0x07:
                    if (ENGINE_state.reg[INS1 & 0x0f] < ENGINE_state.reg[(INS2 & 0xf0) >> 4])
                    {
                        ENGINE_state.reg[0xf] = 1;
                    }
                    else
                    {
                       ENGINE_state.reg[0xf] = 0;
                    }
                    ENGINE_state.reg[INS1 & 0x0f] = ENGINE_state.reg[INS1 & 0x0f] - ENGINE_state.reg[(INS2 & 0xf0) >> 4];
                    break;

                /* SHL  */
                case 0x0e:
                    if (ENGINE_state.reg[INS1 & 0x0f] & 0x8)
                    {
                        ENGINE_state.reg[0xf] = 1;
                    }
                    else
                    {
                       ENGINE_state.reg[0xf] = 0;
                    }
                    ENGINE_state.reg[INS1 & 0x0f] <<= 1;
                    break;
            }
            break;

        /* SNE */
        case 0x90:
            if ((INS2 & 0x0f) == 0)
            {
                if (ENGINE_state.reg[INS1 & 0x0f] !=
                    ENGINE_state.reg[(INS2 & 0xf0) >> 4])
                {
                    advancePC = 4;
                }
            }
            break;

        /* LD */
        case 0xa0:
            ENGINE_state.i = addressTo12bit(ENGINE_state.pc);
            printf("I register: %d\n", ENGINE_state.i);
            break;

        /* JP */
        case 0xb0:
            ENGINE_state.pc = ENGINE_state.reg[0x0] + addressTo12bit(ENGINE_state.pc);
            break;

        /* RND */
        case 0xc0:
            ENGINE_state.reg[INS1 & 0x0f] = SYS_randChar() & INS2;
            break;

        /* DRW */
        case 0xd0:
            printf("Drawing with registers %d and %d, length %d, location = %d\nMEMORY = %p, passed in = %p\n", INS1 & 0x0f, (INS2 & 0xf0) >> 4, INS2 & 0x0f, ENGINE_state.i, ENGINE_state.mem, &ENGINE_state.mem[ENGINE_state.i]);

            printf("~~~SPRITE~~~\n");
            unsigned char thing;
            for (int i = 0; i < (INS2 & 0x0f); i++)
            {
                thing = ENGINE_state.mem[ENGINE_state.i + i];
                for(int j = 0; j < 8; j++)
                {
                    printf((thing & 0x80) ? "*": "-");
                    thing <<= 1;
                }
                printf("\n");
            }
            DISPLAY_drawSprite(ENGINE_state.reg[INS1 & 0x0f],
                    ENGINE_state.reg[(INS2 & 0xf0) >> 4],
                    &ENGINE_state.mem[ENGINE_state.i],
                    INS2 & 0x0f);
            break;

        /* TODO: Stubs, no input yet */
        case 0xe0:
            switch(INS2)
            {
                case 0x9e:
                    break;
                case 0xa1:
                    break;
            }
            break;

        case 0xf0:
            switch(INS2)
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
                    ENGINE_state.i = CHAR_START + ENGINE_state.reg[INS1 & 0x0f] * CHAR_HEIGHT;
                    break;

                /* LD */
                case 0x33:
                    {
                        unsigned char regValue = ENGINE_state.reg[INS1 & 0x0f];
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
//    printf("advancePC = %u\nRegisters: ", advancePC);
//    for (int i = 0; i < 16; i++)
//    {
//        printf("%d = %d,", i, ENGINE_state.reg[i]);
//    }
//    printf("\n");
//    if(ENGINE_state.pc > 4095)
//    {
//        printf("INVALID ADRESS!!!\n");
//        // error!
//    }
//    printf("\n");
}

void ENGINE_run(void)
{
    while (1)
    {
        SDL_Delay(200);
        ENGINE_cycle();
    }
}
