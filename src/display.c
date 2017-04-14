#include "display.h"

/* static SDL variables */
static SDL_Window * DISPLAY_window;
static SDL_Surface * DISPLAY_surfaceWindow;
static SDL_Surface * DISPLAY_surfaceChip;

/* Static vars for the state of the display */
static displayMode DISPLAY_currentDisplayMode;

typedef struct
{
    short width;
    short height;
} displaySize;

static displaySize availableModes[] =
{
    {64 , 32},
    {128, 64},
    {64 , 48},
    {64 , 64}
};

int DISPLAY_init(displayMode mode, short scale)
{
    if (scale == 0)
        scale = 1;

    /* SDL window, renderer and surface setup */
    if (!SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO))
    {
        DISPLAY_window = SDL_CreateWindow("8asil",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                availableModes[mode].width * scale,
                availableModes[mode].height * scale,
                SDL_WINDOW_SHOWN);

        if (DISPLAY_window)
        {

            DISPLAY_surfaceWindow = SDL_GetWindowSurface(DISPLAY_window);
            DISPLAY_surfaceChip = SDL_CreateRGBSurface(0,
                    availableModes[mode].width,
                    availableModes[mode].height,
                    1,
                    0,
                    0,
                    0,
                    0);
            if (DISPLAY_surfaceWindow && DISPLAY_surfaceChip)
            {
                /* TODO: Setup the CHIP's screen custom palette here */

                DISPLAY_currentDisplayMode = mode;
                return 1;
            }

        }

    }

    SDL_Quit();
    return 0;
}

void DISPLAY_clear(void)
{
    /* Pixels are one byte each */
    /* TODO: Set custom colour here instead of zero */
    memset(DISPLAY_surfaceChip->pixels, 0, availableModes[DISPLAY_currentDisplayMode].width
            * availableModes[DISPLAY_currentDisplayMode].height);
    DISPLAY_update();
}

void DISPLAY_drawSprite(char x, char y, char * sprite, char rows)
{

}

void DISPLAY_update(void)
{
    SDL_BlitScaled(DISPLAY_surfaceChip, NULL, DISPLAY_surfaceWindow, NULL);
}
