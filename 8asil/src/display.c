#include "display.h"

/* static SDL variables */
static SDL_Window * DISPLAY_window;
static SDL_Surface * DISPLAY_surfaceWindow;
static SDL_Surface * DISPLAY_surfaceChip;

typedef struct
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} colour;

// colour backgroundColour = {0, 255, 0};
// colour foregroundColour = ;
//
//foregroundColour.red = 0;
//foregroundColour.green = 255;
//foregroundColour.blue = 0;

SDL_Color colors[256];



unsigned char topCol;
unsigned char botCol;

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
                    8,
                    0,
                    0,
                    0,
                    0);
            if (DISPLAY_surfaceWindow && DISPLAY_surfaceChip)
            {
                /* TODO: Setup the CHIP's screen custom palette here */

                DISPLAY_currentDisplayMode = mode;
                printf("Successfully initialized screen\n");
                
                for(int i = 0; i < 256; i++)
                {
                    colors[i].r = colors[i].g = colors[i].b = (Uint8)i;
                    if (i == 0)
                    {
                        colors[i].r = 148; colors[i].g = 208; colors[i].b = 255;
                    } else if (i == 1)
                    {
                        colors[i].r = 199; colors[i].g = 116; colors[i].b = 232;
                    }
                }
                
                SDL_SetSurfaceBlendMode(DISPLAY_surfaceWindow, SDL_BLENDMODE_NONE);
                SDL_SetSurfaceBlendMode(DISPLAY_surfaceChip, SDL_BLENDMODE_NONE);
                SDL_SetPaletteColors(DISPLAY_surfaceChip->format->palette, colors, 0, 256);
//                printf("BLACK IS %d, WHITE IS %d\n",
//                       SDL_MapRGB(DISPLAY_surfaceChip->format,255, 0, 255),
//                       SDL_MapRGB(DISPLAY_surfaceChip->format,0, 255, 0));
                
                topCol = SDL_MapRGB(DISPLAY_surfaceChip->format, 148,208,255);
                botCol = SDL_MapRGB(DISPLAY_surfaceChip->format, 199,116,232);
                
                
                return 1;
            }

        }

    }

    SDL_Quit();
    return 0;
}

static void DISPLAY_debug(void)
{
    return;
    printf("Pixel = %p\n", (unsigned char *)(DISPLAY_surfaceChip->pixels));
    unsigned char *beepboop =(unsigned char *)(DISPLAY_surfaceChip->pixels);
    for (unsigned char i = 0; i < availableModes[DISPLAY_currentDisplayMode].height; i++)
    {
        for(unsigned char j = 0; j < availableModes[DISPLAY_currentDisplayMode].width; j++)
        {
            if (beepboop[i * availableModes[DISPLAY_currentDisplayMode].width + j] == topCol)
            {
                printf("*");
            }
            else if (beepboop[i * availableModes[DISPLAY_currentDisplayMode].width + j] == botCol)
            {
                printf("-");
            }
            else
            {
                printf("?");
            }
        }
        printf("\n");
    }
}

void DISPLAY_clear(void)
{
//    printf("DISPLAY_clear called!\n");
    DISPLAY_debug();
    /* Pixels are one byte each */
    /* TODO: Set custom colour here instead of zero */
    
    unsigned char * pixel = (unsigned char *)(DISPLAY_surfaceChip->pixels);
    for (int i = 0; i < availableModes[DISPLAY_currentDisplayMode].width * availableModes[DISPLAY_currentDisplayMode].height; i++)
    {
        if (i > 2047)
        {
            printf("WRONG\n");
        }

        pixel[i] = botCol;
    }
//    printf("AFTER CLEAR:\n");
DISPLAY_debug();
    DISPLAY_update();
}

unsigned char DISPLAY_drawSprite(char x, char y, unsigned char * sprite, char rows)
{
    unsigned short pixerased = 0;
//    printf("DISPLAY_drawSprite called! x = %u, y = %u, rows = %u\n", x, y, rows);
    unsigned short currentX = x % availableModes[DISPLAY_currentDisplayMode].width;
    unsigned short currentY = y % availableModes[DISPLAY_currentDisplayMode].height;
    unsigned char * pixelLocation = (unsigned char *)(DISPLAY_surfaceChip->pixels);
    unsigned char spritebyte = *sprite;
    unsigned char spritebit;
    unsigned char screenbit;
    
    for (unsigned char i = 0; i < rows; i++)
    {
        for (unsigned char j = 0; j < 8; j++)
        {
            if (currentX + currentY * availableModes[DISPLAY_currentDisplayMode].width > 2047)
            {
                printf("WRONG\n");
            }

            screenbit = pixelLocation[currentX + currentY * availableModes[DISPLAY_currentDisplayMode].width] == topCol ? 1 : 0;
            spritebit = (spritebyte << j) & 0x80 ? 1 : 0;
            
            if (screenbit ^ spritebit)
            {
                pixelLocation[currentX + currentY * availableModes[DISPLAY_currentDisplayMode].width] = topCol;
            }
            else
            {
                if (screenbit)
                {
                    pixerased = 1;
                }
                pixelLocation[currentX + currentY * availableModes[DISPLAY_currentDisplayMode].width] = botCol;
            }
            
            currentX = (currentX + 1) % availableModes[DISPLAY_currentDisplayMode].width;
        }
        spritebyte = *(sprite + i + 1);
        currentY = (currentY + 1) % availableModes[DISPLAY_currentDisplayMode].height;
        currentX = x % availableModes[DISPLAY_currentDisplayMode].width;
    }

    
    DISPLAY_update();
    
    return pixerased;
}



void DISPLAY_update(void)
{
//    printf("DISPLAY_update called!\n");
//    printf("BEFORE:\n");
//    DISPLAY_debug();
//    DISPLAY_debug();
    SDL_Rect stretchRect;
				stretchRect.x = 0;
				stretchRect.y = 0;
				stretchRect.w = 511;
				stretchRect.h = 255;
    
    SDL_FillRect(DISPLAY_surfaceWindow, &DISPLAY_surfaceWindow->clip_rect, SDL_MapRGBA(DISPLAY_surfaceChip->format, 255, 0, 0, 255));
    
    SDL_Surface * asdf = SDL_ConvertSurface(DISPLAY_surfaceChip, DISPLAY_surfaceWindow->format, 0);
    
//    SDL_BlitSurface(DISPLAY_surfaceChip, NULL, DISPLAY_surfaceWindow, NULL);
    SDL_BlitScaled(asdf, NULL, DISPLAY_surfaceWindow, NULL);
//    DISPLAY_debug();
    SDL_UpdateWindowSurface(DISPLAY_window);
//    DISPLAY_debug();
//    printf("AFTER:\n");
    DISPLAY_debug();
}
