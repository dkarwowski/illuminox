#include <SDL2/SDL.h>
#include "main.h"
#include "game.h"

/**
 * Update the game state and objects
 * @memory : the memory we keep constant
 * @input  : input from the main
 *
 * This can run many times before rendering, or just once. Requires some
 * MS_PER_UPDATE to be defined.
 */
extern
UPDATE(Update) /* memory, input */
{
    struct GameState *state = (struct GameState *)memory->perm_mem;

    if (!state->init) {
        state->init = true;
        memset(&state->tiles, 0, sizeof(u8));

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (j == 0 || j == 9 || i == 0 || i == 9)
                    state->tiles[i * 10 + j] = 1;
            }
        }
    }
}

/**
 * Render the actual scene onto the screen
 * @memory   : struct of the actual memory
 * @renderer : sdl renderer to display to
 * @dt       : the time with which to adjust when rendering mid-update frame
 *
 * We have to adjust positions slightly based on velocity to get the right
 * render position for each object when doing something mid-frame
 */
extern
RENDER(Render) /* memory, renderer, dt */
{
    /* has been initialized in update */
    struct GameState *state = (struct GameState *)memory->perm_mem;

    SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
    SDL_RenderClear(renderer);

    SDL_Rect rect = { 10, 10, 40, 40 };
    for (int i = 0; i < 10; i++) {
        rect.x = 10;
        for (int j = 0; j < 10; j++) {
            if (state->tiles[i * 10 + j])
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            else
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &rect);
            rect.x += rect.w;
        }
        rect.y += rect.h;
    }

    SDL_RenderPresent(renderer);
}

