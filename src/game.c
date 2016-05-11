#include <SDL2/SDL.h>
#include "main.h"
#include "game.h"

extern
UPDATE(Update) /* memory, input */
{
    struct GameState *state = (struct GameState *)memory;

    if (!state->init) {
        state->init = true;
    }
}

extern
RENDER(Render) /* memory, renderer, dt */
{
    SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

