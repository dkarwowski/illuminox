#include <SDL2/SDL.h>

#ifdef WIN_BUILD
#include <windows.h>
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <dlfcn.h>
#endif

#include "main.h"

static
int
handle_event(SDL_Event *event, struct GameInput *old_input, struct GameInput *new_input)
{
    switch (event->type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            bool was_down = event->key.repeat != 0 || event->key.state == SDL_RELEASED;
            bool is_down  = event->key.repeat != 0 || event->key.state == SDL_PRESSED;

            if (was_down != is_down) {
                /* iterate over keys and set new values in input */
            }
        } break;
        case SDL_QUIT:
        {
            return -1;
        } break;
        default:
            break;
    }

    return 0;
}
