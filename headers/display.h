#include <SDL2/SDL.h>

typedef enum
{
    CHIP = 0,
    SUPERCHIP,
    ETI48,
    ETI64
} displayMode;

/* DISPLAY_init
 * Initialize the display's resolution and the display size (scale)
 * TODO: Perhaps change scale parameter, make it more configurable
 * Scaling would also act weird and be too big if it's SuperCHIP8
 */
int DISPLAY_init(displayMode mode, short scale);

/* DISPLAY_clear
 * Clears the display
 */
void DISPLAY_clear(void);

/* DISPLAY_drawSprite
 * Draws a sprite to the screen using a memory location, up to 8x15
 * Used for the DRW instruction
 */
void DISPLAY_drawSprite(char x, char y, char * sprite, char rows);

/* DISPLAY_update
 * Update the display's surface what's in the CHIP's surface
 */
void DISPLAY_update(void);

/* DISPLAY_close
 * Closes display and does cleanup
 */
void DISPLAY_close(void);
