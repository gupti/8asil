/* Temporary for testing, engine.c will take care of this */
#include <engine.h>

int main(int argc, char* argv[])
{
    ENGINE_start("/Users/paul/test.c8");
    
//    SDL_Event event;
    
    while (1)
    {
        ENGINE_run();
//        SDL_PollEvent(&event);
//        if(event.type == SDL_QUIT)
//        {
//            break;
//        }
        SDL_Delay(100);
    }
    
    return 0;
}
