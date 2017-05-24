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


void SYS_setDelay(unsigned char delay);

unsigned char SYS_getDelay(void);

void SYS_setSound(unsigned char audio);

unsigned char SYS_waitForKey(void);

unsigned char SYS_getKeyState(unsigned char key);