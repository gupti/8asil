#include "system.h"

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
