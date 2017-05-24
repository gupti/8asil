#include "display.h"
#include "system.h"

/* ENGINE_start
 * Initialization function of the engine that starts emulation
 */
void ENGINE_start(char * progName);

/* ENGINE_run
 * Resumes code execution
 */
void ENGINE_run(void);

/* ENGINE_pause
 * Pauses code excution
 */
void ENGINE_pause(void);

/* ENGINE_reset
 * Resets the machine's start back to the start
 */
void ENGINE_reset(void);

