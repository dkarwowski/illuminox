#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <math.h>

#include "main.h"
#include "game.h"

/* Compare macro to make it more legible */
#define I_COMPARE(input_text, command) (strcmp(input_text + 2, command) == 0)

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
I_ExecuteCommand(struct GameState *state, struct GameInput *input)
{
    if (I_COMPARE(input->input_text, "reload")) {
        input->reload_lib = true;
    } else if (I_COMPARE(input->input_text, "restart")) {
        input->reload_lib = true;
        state->init = false;
    } else if (I_COMPARE(input->input_text, "quit")) {
        input->quit.was_down = true;
    } else if (I_COMPARE(input->input_text, "clear")) {
        for (int i = 0; i < 10; state->buffer[i++][0] = '\0') /* that's it */;
        input->input_text[2] = '\0';
        input->input_len = 2;
    } else if (I_COMPARE(input->input_text, "")) {
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

void
Move(struct WorldState *world, struct Entity *ent, struct Vec2 acc)
{
    ent->vel = V2_Add(V2_Mul(0.95f, ent->vel), V2_Mul(SEC_PER_UPDATE, acc));

    /* don't set the position until after we check collisions */
    struct Vec2 dpos = V2_Mul(SEC_PER_UPDATE, ent->vel);
    struct Vec2 npos = V2_Add(ent->pos, dpos);

    int min_tilex, max_tilex, min_tiley, max_tiley;
    min_tilex = (int)MIN(npos.x - ent->rad.w, ent->pos.x - ent->rad.w);
    min_tiley = (int)MIN(npos.y - ent->rad.h, ent->pos.y - ent->rad.h);
    max_tilex = (int)MAX(npos.x + ent->rad.w, ent->pos.x + ent->rad.w);
    max_tiley = (int)MAX(npos.y + ent->rad.h, ent->pos.y + ent->rad.h);

    /* actual collision detection and handling */
    r32 tleft = 1.0f;
    for (int z = 0; z < 4 && tleft > 0.0f; z++) {
        struct Vec2 normal = {0.0f, 0.0f};
        r32 tmin = 1.0f;
        for (int i = min_tiley; i <= max_tiley; i++) {
            for (int j = min_tilex; j <= max_tilex; j++) {
                if (i < 0 || i > 9 || j < 0 || j > 9) continue;
                if (world->chunks[0].tiles[i * 10 + j] == 0) continue;
                /* points with small epsilon for flush collision */
                r32 points[] = { (r32)j - ent->rad.w        - ent->pos.x,
                                 (r32)i - ent->rad.h        - ent->pos.y,
                                 (r32)j + ent->rad.w + 1.0f - ent->pos.x,
                                 (r32)i + ent->rad.h + 1.0f - ent->pos.y };
                /* loop over walls defined in this way (top, bottom, left, right) */
                struct {
                    r32 x0, x1, y, dy, dx;
                    struct Vec2 normal;
                } walls[] = {{ points[0], points[2], points[1], dpos.y, dpos.x, {  0.0f, -1.0f } },
                             { points[0], points[2], points[3], dpos.y, dpos.x, {  0.0f,  1.0f } },
                             { points[1], points[3], points[0], dpos.x, dpos.y, { -1.0f,  0.0f } },
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

        /* adjust old pos with some sort of normal */
        ent->pos = V2_Add(ent->pos, V2_Mul(tmin, dpos));
        ent->vel = V2_Sub(ent->vel, V2_Mul(V2_Dot(ent->vel, normal), normal));
        dpos = V2_Sub(dpos, V2_Mul(V2_Dot(dpos, normal), normal));
        tleft -= tmin;
    }
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

        struct WorldChunk *chunk = &state->world.chunks[0];
        chunk->x = 0;
        chunk->y = 0;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (j == 0 || j == 9 || i == 0 || i == 9)
                    chunk->tiles[i * 10 + j] = 1;
                else
                    chunk->tiles[i * 10 + j] = 0;
            }
        }
        chunk->next = NULL;

        state->player.worldx = 0;
        state->player.worldy = 0;
        state->player.pos = (struct Vec2){ 5.0f, 5.0f };
        state->player.vel = (struct Vec2){ 0.0f, 0.0f };
        state->player.rad = (struct Vec2){ 0.45f, 1.2f };

        /* Initialize font as best possible, if it fails then ensure it's NULL */
        if (!TTF_WasInit()) {
            if (TTF_Init() == -1) {
                state->font = NULL;
                fprintf(stderr, "Can't initialize TTF\n");
            } else {
                state->font = TTF_OpenFont("../res/VeraMono.ttf", 12);
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
        I_ExecuteCommand(state, input);
    } else {
        input->input_entered = false;
    }

    /* always perform this copy, ensures our final buffer is updated */
    memcpy(state->buffer[0], input->input_text, input->input_len + 1);

    /* handle everything for quitting out immediately */
    if (I_IsPressed(&input->quit)) {
        TTF_CloseFont(state->font);
        state->font = NULL;
        TTF_Quit();
        return;
    }

    /* toggle whether we're using the console or not */
    if (I_IsToggled(&input->console)) {
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

    /* Handle player ------------------------------------------------------ */
    /* adjust the player movement */
    struct Vec2 acc = { 0.0f, 0.0f };
    if (I_IsPressed(&input->move_down)) {
        acc.y += 1.0f;
    }
    if (I_IsPressed(&input->move_up)) {
        acc.y -= 1.0f;
    }
    if (I_IsPressed(&input->move_right)) {
        acc.x += 1.0f;
    }
    if (I_IsPressed(&input->move_left)) {
        acc.x -= 1.0f;
    }
    acc = V2_Mul(25.0f, V2_Norm(acc));

    Move(&state->world, &state->player, acc);
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

    SDL_Surface *temp;
    if (state->floor_texture == NULL) {
        temp = SDL_LoadBMP("../res/tiles/tile_floor.bmp");
        SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 255, 0, 255));
        state->floor_texture = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);
    }
    if (state->wall_texture == NULL) {
        temp = SDL_LoadBMP("../res/tiles/tile_wall.bmp");
        SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 255, 0, 255));
        state->wall_texture = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);
    }

    SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
    SDL_RenderClear(renderer);

    /* render the tiles */
    SDL_Rect rect = { 0, 0, PIXEL_PERMETERX, PIXEL_PERMETERY };
    for (int i = 0; i < 10; i++) {
        rect.x = 0;
        for (int j = 0; j < 10; j++) {
            if (state->world.chunks[0].tiles[i * 10 + j])
                SDL_RenderCopy(renderer, state->wall_texture, NULL, &(SDL_Rect){ rect.x, rect.y - PIXEL_PERMETERY, rect.w, rect.h*2 });
            else
                SDL_RenderCopy(renderer, state->floor_texture, NULL, &rect);
            rect.x += PIXEL_PERMETERX;
        }
        rect.y += PIXEL_PERMETERY;
    }

    /* render the player */
    struct Entity *ent = &state->player;
    SDL_Rect player_rect = { (int)(PIXEL_PERMETERX * (ent->pos.x + ent->vel.x * dt - ent->rad.w) + 0.5f),
                             (int)(PIXEL_PERMETERY * (ent->pos.y + ent->vel.y * dt - ent->rad.h) + 0.5f),
                             (int)(PIXEL_PERMETERX * ent->rad.w * 2.0f + 0.5f),
                             (int)(PIXEL_PERMETERY * ent->rad.h * 2.0f + 0.5f) };
    SDL_SetRenderDrawColor(renderer, 125, 0, 125, 255);
    SDL_RenderFillRect(renderer, &player_rect);

//    /* square to show the movement vactor is working */
//    SDL_Rect move_vector = { (int)(PIXEL_PERMETER * state->pos.x),
//                             (int)(PIXEL_PERMETER * state->pos.y),
//                             (int)(PIXEL_PERMETER * state->vel.x) + 2,
//                             (int)(PIXEL_PERMETER * state->vel.y) + 2 };
//    SDL_SetRenderDrawColor(renderer, 0, 125, 125, 255);
//    SDL_RenderFillRect(renderer, &move_vector);

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
                SDL_Rect r = { 25, height - 25 - TTF_FontLineSkip(state->font) * (i+1), tw, th };
                SDL_RenderCopy(renderer, texture, NULL, &r);
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

