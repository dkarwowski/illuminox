#include <SDL2/SDL.h>
#include "main.h"

extern
void
UpdateAndRender(struct GameMemory *memory, struct GameInput *input, SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

