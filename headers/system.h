#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* SYS_init
 * Initializes system related things, such as PRNG seed
 */
void SYS_init(void);

/* SYS_loadProgram
 * Loads a program from its name into memLocation with a limit on length
 */
int SYS_loadProgram(char * progName, void * memLoc, size_t max);

/* SYS_randChar
 * Produces a random number from 0 to 255
 */
unsigned char SYS_randChar(void);

/* SYS_setDelay
 * Sets delay timer, value from 0 to 255
 */
void SYS_setDelay(unsigned char delay);

/* SYS_getDelay
 * Gets the current delay timer value
 */
unsigned char SYS_getDelay(void);

/* SYS_setSound
 * Sets sound timer, value from 0 to 255
 */
void SYS_setSound(unsigned char audio);

/* SYS_watiForKey
 * Waits for valid keypress, then returns key value (0-15)
 */
unsigned char SYS_waitForKey(void);

/* SYS_getKeyState
 * Gets key state for any key from 0 to 15. 1 if pressed, 0 if not
 */
unsigned char SYS_getKeyState(unsigned char key);

