#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <math.h>

#include "main.h"
#include "game.h"

/* Compare macro to make it more legible */
#define C_COMPARE(input_text, command) (strcmp(input_text + 2, command) == 0)

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)
#define SIGN(a) ((0.0001f < a) - (a < -0.0001f))

/**
 * Execute a console command that was entered
 *
 * @state : the current game state in case it needs to be manipulated
 * @input : the current state of the input keys
 *
 * The current command should still be stored in the input->input_text
 * field. It'll work either way, just come out as empty space. Any
 * command that isn't handled is replaced with invalid.
 */
static
void
C_ExecuteCommand(struct GameState *state, struct GameInput *input)
{
    if (C_COMPARE(input->input_text, "reload")) {
        input->reload_lib = true;
    } else if (C_COMPARE(input->input_text, "restart")) {
        input->reload_lib = true;
        state->init = false;
    } else if (C_COMPARE(input->input_text, "quit")) {
        input->quit.was_down = true;
    } else if (C_COMPARE(input->input_text, "clear")) {
        for (int i = 0; i < 10; state->buffer[i++][0] = '\0') /* that's it */;
        input->input_text[2] = '\0';
        input->input_len = 2;
    } else if (C_COMPARE(input->input_text, "")) {
        /* do nothing */
    } else { /* text entered was invalid, so say so */
        memcpy(input->input_text + 2, "invalid", 8);
        input->input_len = 10;
    }

    memcpy(state->buffer[1], input->input_text + 2, input->input_len - 1);

    /* cleanup the input text now */
    input->input_entered = false;
    input->input_text[2] = '\0';
    input->input_len = 2;
}

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
        SDL_StopTextInput();
        input->input_text[2] = '\0';
        input->input_len = 2;

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (j == 0 || j == 9 || i == 0 || i == 9)
                    state->tiles[i * 10 + j] = 1;
                else
                    state->tiles[i * 10 + j] = 0;
            }
        }

        state->pos = (struct Vec2){ 5.0f, 5.0f };
        state->vel = (struct Vec2){ 0.0f, 0.0f };

        /* Initialize font as best possible, if it fails then ensure it's NULL */
        if (!TTF_WasInit()) {
            if (TTF_Init() == -1) {
                state->font = NULL;
                fprintf(stderr, "Can't initialize TTF\n");
            } else {
                state->font = TTF_OpenFont("../res/VeraMono.ttf", 26);
                if (state->font == NULL)
                    fprintf(stderr, "Can't open the font: %s\n", TTF_GetError());
            }
        }

        /* TODO(david): remove hard coded buffer length */
        for (int i = 0; i < 10; i++) {
            state->buffer[i][0] = '\0';
        }
    }

    /* Handle Input ------------------------------------------------------- */
    if (input->input_entered && input->input_len > 0) {
        for (int i = 9; i > 1; i--)
            memcpy(state->buffer[i], state->buffer[i-1], sizeof(state->buffer[i]));
        C_ExecuteCommand(state, input);
    } else {
        input->input_entered = false;
    }

    /* always perform this copy, ensures our final buffer is updated */
    memcpy(state->buffer[0], input->input_text, input->input_len + 1);

    /* handle everything for quitting out immediately */
    if (C_IsPressed(&input->quit)) {
        TTF_CloseFont(state->font);
        state->font = NULL;
        TTF_Quit();

        return;
    }

    if (C_IsToggled(&input->console)) {
        state->console = !state->console;
        if (state->console)
            SDL_StartTextInput();
        else
            SDL_StopTextInput();
    }

    /* we don't want to actually do any of the real updating if the console
     * is currently running. This allows for us to make changes and then,
     * in a sense, resume the game with those changes in place */
    if (state->console)
        return;

    /* adjust the player movement */
    struct Vec2 acc = { 0.0f, 0.0f };
    if (C_IsPressed(&input->move_down)) {
        acc.y += 1.0f;
    }
    if (C_IsPressed(&input->move_up)) {
        acc.y -= 1.0f;
    }
    if (C_IsPressed(&input->move_right)) {
        acc.x += 1.0f;
    }
    if (C_IsPressed(&input->move_left)) {
        acc.x -= 1.0f;
    }
    acc = V2_Mul(25.0f, V2_Norm(acc));
    state->vel = V2_Add(V2_Mul(0.95f, state->vel), V2_Mul(SEC_PER_UPDATE, acc));

    /*
    struct Vec2 dpos = V2_Add(V2_Mul((r32)(SEC_PER_UPDATE*SEC_PER_UPDATE)/2.0f, acc),
                              V2_Mul(SEC_PER_UPDATE, state->vel));
                              */
    struct Vec2 dpos = V2_Mul(SEC_PER_UPDATE, state->vel);

    struct Vec2 npos = V2_Add(state->pos, dpos);

    int min_tilex, max_tilex, min_tiley, max_tiley;
    min_tilex = (int)MIN(npos.x - 0.45f, state->pos.x - 0.45f);
    max_tilex = (int)MAX(npos.x + 0.45f, state->pos.x + 0.45f);
    min_tiley = (int)MIN(npos.y - 0.6f,  state->pos.y - 0.6f);
    max_tiley = (int)MAX(npos.y + 0.6f,  state->pos.y + 0.6f);

    r32 tleft = 1.0f;
    for (int z = 0; z < 4 && tleft > 0.0f; z++) {
        struct Vec2 normal = {0.0f, 0.0f};
        r32 tmin = 1.0f;
        for (int i = min_tiley; i <= max_tiley; i++) {
            for (int j = min_tilex; j <= max_tilex; j++) {
                if (i < 0 || i > 9 || j < 0 || j > 9) continue;
                if (state->tiles[i * 10 + j] == 0) continue;
                r32 points[] = { (r32)j - 0.4499f - state->pos.x,
                                 (r32)i - 0.5999f - state->pos.y,
                                 (r32)j + 1.4499f - state->pos.x,
                                 (r32)i + 1.5999f - state->pos.y };
                struct {
                    r32 x0, x1, y, dy, dx;
                    struct Vec2 normal;
                } walls[] = {{ points[1], points[3], points[0], dpos.x, dpos.y, { -1.0f,  0.0f } },
                             { points[0], points[2], points[1], dpos.y, dpos.x, {  0.0f, -1.0f } },
                             { points[0], points[2], points[3], dpos.y, dpos.x, {  0.0f,  1.0f } },
                             { points[1], points[3], points[2], dpos.x, dpos.y, {  1.0f,  0.0f } }};

                for (int walli = 0; walli < 4; walli++) {
                    r32 epsilon = 0.001f;
                    if (fabsf(walls[walli].dy) > 0.001f) {
                        r32 t = walls[walli].y / walls[walli].dy;
                        r32 x = t * walls[walli].dx;
                        if (t > 0.0f && walls[walli].x0 < x && x < walls[walli].x1 && tmin > t) {
                            tmin = MAX(0.0f, t - epsilon);
                            normal = walls[walli].normal;
                        }
                    }
                }
            }
        }
        state->pos = V2_Add(state->pos, V2_Mul(tmin, dpos));
        state->vel = V2_Sub(state->vel, V2_Mul(V2_Dot(state->vel, normal), normal));
        dpos = V2_Sub(dpos, V2_Mul(V2_Dot(dpos, normal), normal));
        tleft -= tmin;
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

    /* render the tiles */
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

    /* render the player */
    SDL_Rect player_rect = { (int)(PIXEL_PERMETER * (state->pos.x + state->vel.x * dt - 0.45f)),
                             (int)(PIXEL_PERMETER * (state->pos.y + state->vel.y * dt - 0.6f)),
                             (int)(PIXEL_PERMETER * 0.9f),
                             (int)(PIXEL_PERMETER * 1.2f) };
    SDL_SetRenderDrawColor(renderer, 125, 0, 125, 255);
    SDL_RenderFillRect(renderer, &player_rect);

    SDL_SetRenderDrawColor(renderer, 0, 125, 125, 255);
    SDL_RenderDrawLine(renderer, (int)(PIXEL_PERMETER * state->pos.x),
                                 (int)(PIXEL_PERMETER * state->pos.y),
                                 (int)(PIXEL_PERMETER * (state->pos.x + state->vel.x)),
                                 (int)(PIXEL_PERMETER * (state->pos.y + state->vel.y)));

    if (state->console && state->font != NULL) {
        int width, height;
        SDL_RenderGetLogicalSize(renderer, &width, &height);
        int line_height = TTF_FontLineSkip(state->font);
        SDL_Rect rect_console = { 20, height - 20 - line_height * 10 - 10, width - 40, line_height * 10 + 10 };
        SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
        SDL_RenderFillRect(renderer, &rect_console);

        SDL_Color fg_color = { 255, 255, 255, 255 };
        SDL_Color bg_color = {  10,  10,  10, 255 };
        for (int i = 0; i < 10; i++) {
            if (state->buffer[i][0] != '\0') {
                SDL_Surface *surface = TTF_RenderText_Shaded(state->font, state->buffer[i], fg_color, bg_color);
                SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
                int tw, th;
                SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
                SDL_Rect r = { 25, height - 25 - TTF_FontLineSkip(state->font) * (i+1), tw + 1, th };
                SDL_RenderCopy(renderer, texture, NULL, &r);
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

