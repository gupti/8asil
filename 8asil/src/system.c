#include "system.h"
#include <SDL2/SDL.h>

/* Delay and sound timers */
static clock_t delayTimer;
static clock_t soundTimer;
static SDL_Scancode keyMap[] = {SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
                                SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
                                SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
                                SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V};

void SYS_init(void)
{
    srand(time(NULL));
}

int SYS_loadProgram(char * progName, void * memLoc, size_t max)
{
    FILE * program = fopen(progName, "r");
    
    /* File does not exist */
    if (!program)
    {
        return 1;
    }

    fseek(program, 0, SEEK_END);
    size_t fileSize = ftell(program);

    /* Program too large to load into memory */
    if (fileSize > max)
    {
        return 2;
    }
    rewind(program);
    if(!fread(memLoc, fileSize, sizeof(unsigned char), program))
    {
        printf("READ ERROR!\n");
        return 3;
    }
    fclose(program);

    printf("SUCESSFUL LOAD\n");
    return 0;
}

unsigned char SYS_randChar(void)
{
    return rand() % (UCHAR_MAX + 1);
}


void SYS_setDelay(unsigned char delay)
{
    delayTimer = clock() + (delay * CLOCKS_PER_SEC) / 60;
}

unsigned char SYS_getDelay(void)
{
    clock_t currentClock = clock();
    if (delayTimer > currentClock || currentClock - (UCHAR_MAX * CLOCKS_PER_SEC) / 60 > delayTimer)
    {
        return delayTimer - currentClock;
    }
    return 0;
}

/* TODO: function is a stub for now */
void SYS_setSound(unsigned char audio)
{
    soundTimer = clock();
}

unsigned char SYS_waitForKey(void)
{
    SDL_Event event;
    while (SDL_WaitEvent(&event))
    {
        if (event.type == SDL_KEYDOWN)
        {
            for (int i = 0; i < 16; i++)
            {
                if (keyMap[i] == event.key.keysym.scancode)
                {
                    return i;
                }
            }
        }
    }
    return 0;
}

unsigned char SYS_getKeyState(unsigned char key)
{
    return (unsigned char)SDL_GetKeyboardState(NULL)[keyMap[key]];
}