#include <engine.h>

int main(int argc, char* argv[])
{
    if (argc > 1)
    {
	SDL_Event event;
        ENGINE_start(argv[1]);

        while (1)
        {
            ENGINE_run();

            SDL_PollEvent(&event);
            if(event.type == SDL_QUIT)
            {
                break;
            }

            /* TODO: fix sketchy rate limiting */
            SDL_Delay(5);
        }
    }
    else
    {
        printf("No game.\n");
    }

    return 0;
}

