#include <SDL2/SDL_ttf.h>
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
        state->console = false;

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (j == 0 || j == 9 || i == 0 || i == 9)
                    state->tiles[i * 10 + j] = 1;
                else
                    state->tiles[i * 10 + j] = 0;
            }
        }

        /** Initialize font as best possible, if it fails then ensure it's NULL */
        if (TTF_Init() == -1) {
            state->font = NULL;
            fprintf(stderr, "Can't initialize TTF\n");
        } else {
            state->font = TTF_OpenFont("../res/VeraMono.ttf", 26);
            if (state->font == NULL)
                fprintf(stderr, "Can't open the font: %s\n", TTF_GetError());
        }
    }

    if (C_IsToggled(&input->console))
        state->console = !state->console;
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

    SDL_Rect rect = { 0, 0, PIXEL_PERMETER, PIXEL_PERMETER };
    for (int i = 0; i < 10; i++) {
        rect.x = 0;
        for (int j = 0; j < 10; j++) {
            if (state->tiles[i * 10 + j])
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            else
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &rect);
            rect.x += PIXEL_PERMETER;
        }
        rect.y += PIXEL_PERMETER;
    }

    if (state->console) {
        int width, height;
        SDL_RenderGetLogicalSize(renderer, &width, &height);
        SDL_Rect rect_console = { 20, height - 20 - height / 2, width - 40, height / 2 };
        SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
        SDL_RenderFillRect(renderer, &rect_console);
    }

    SDL_RenderPresent(renderer);
}

