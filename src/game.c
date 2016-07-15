#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>

#include "main.h"
#include "math.h"
#include "render_config.h"
#include "world.h"

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

/**
 * Move an entity with a specific acceleration.
 *
 * @world : the current world
 * @ent   : entity that's being moved
 * @acc   : the acceleration we move by
 *
 * The entity should know exactly where it is in the world, as well as its
 * world chunk so that it knows what entities to check.
 */
void
Move(struct WorldState *world, struct Entity *ent, struct Vec2 acc)
{
    ent->vel = V2_Add(V2_Mul(0.95f, ent->vel), V2_Mul(SEC_PER_UPDATE, acc));

    /* don't set the position until after we check collisions */
    struct Vec2 dpos = V2_Mul(SEC_PER_UPDATE, ent->vel);

    /* actual collision detection and handling */
    r32 tleft = 1.0f;
    for (int z = 0; z < 4 && tleft > 0.0f; z++) {
        struct Vec2 normal = {0.0f, 0.0f};
        r32 tmin = 1.0f;
        for (struct Entity *cmp_ent = ent->chunk->head; cmp_ent != NULL; cmp_ent = cmp_ent->next) {
            if (cmp_ent == ent)
                continue;

            /* points with small epsilon for flush collision */
            r32 points_y[] = { cmp_ent->pos.x - cmp_ent->rad.x - ent->rad.x - ent->pos.x,
                               cmp_ent->pos.y - cmp_ent->rad.y - ent->rad.y - ent->pos.y,
                               cmp_ent->pos.x + cmp_ent->rad.x + ent->rad.x - ent->pos.x,
                               cmp_ent->pos.y + cmp_ent->rad.y + ent->rad.y - ent->pos.y };
            /* loop over walls defined in this way (top, bottom, left, right) */
            struct {
                r32 x0, x1, y, dy, dx;
                struct Vec2 normal;
            } walls_y[] = {{ points_y[0], points_y[2], points_y[1], dpos.y, dpos.x, {  0.0f, -1.0f } },
                           { points_y[0], points_y[2], points_y[3], dpos.y, dpos.x, {  0.0f,  1.0f } },
                           { points_y[1], points_y[3], points_y[0], dpos.x, dpos.y, { -1.0f,  0.0f } },
                           { points_y[1], points_y[3], points_y[2], dpos.x, dpos.y, {  1.0f,  0.0f } }};

            for (int walli = 0; walli < 4; walli++) {
                r32 epsilon = 0.001f;
                if (fabsf(walls_y[walli].dy) > 0.001f) {
                    r32 t = walls_y[walli].y / walls_y[walli].dy;
                    r32 x = t * walls_y[walli].dx;
                    if (t > 0.0f && walls_y[walli].x0 < x && x < walls_y[walli].x1 && tmin > t) {
                        tmin = MAX(0.0f, t - epsilon);
                        normal = walls_y[walli].normal;
                    }
                }
            }

            /* points with small epsilon for flush collision */
            r32 points_z[] = { cmp_ent->pos.x - cmp_ent->tl_point.x - ent->tl_point.w - ent->pos.x,
                               cmp_ent->pos.y - cmp_ent->tl_point.y - ent->tl_point.h - ent->pos.y,
                               cmp_ent->pos.x + cmp_ent->br_point.x + ent->br_point.w - ent->pos.x,
                               cmp_ent->pos.y + cmp_ent->br_point.y + ent->br_point.h - ent->pos.y };
            /* loop over walls defined in this way (top, bottom, left, right) */
            struct {
                r32 x0, x1, y, dy, dx;
                struct Vec2 normal;
            } walls_z[] = {{ points_z[0], points_z[2], points_z[1], dpos.y, dpos.x, {  0.0f, -1.0f } },
                           { points_z[0], points_z[2], points_z[3], dpos.y, dpos.x, {  0.0f,  1.0f } },
                           { points_z[1], points_z[3], points_z[0], dpos.x, dpos.y, { -1.0f,  0.0f } },
                           { points_z[1], points_z[3], points_z[2], dpos.x, dpos.y, {  1.0f,  0.0f } }};

            for (int walli = 0; walli < 4; walli++) {
                if (fabsf(walls_z[walli].dy) > 0.001f) {
                    r32 t = walls_z[walli].y / walls_z[walli].dy;
                    r32 x = t * walls_z[walli].dx;
                    if (t > 0.0f && walls_z[walli].x0 < x && x < walls_z[walli].x1 && tmin > t) {
                        /* TODO(david): handle a collision */
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

    struct WorldChunk *new_chunk = W_FixChunk(world, ent->chunk, &ent->pos);
    if (new_chunk != ent->chunk) {
        W_ChunkRemoveEntity(ent->chunk, ent);
        W_ChunkAddEntity(new_chunk, ent);
        ent->chunk = new_chunk;
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

        state->num_ents = 0;

        Z_InitStack( &state->game_stack, 
                     memory->perm_mem + sizeof(struct GameState),
                     memory->perm_memsize - sizeof(struct GameState) );
        Z_InitStack(&state->temp_stack, memory->temp_mem, memory->temp_memsize);

        /* TODO(david): change the way the stack is set up */
        state->world = Z_PushStruct(&state->game_stack, struct WorldState, true);
        Z_InitSubStack( &state->world->stack,
                        &state->game_stack,
                        Z_RemainingStack(&state->game_stack) );

        W_GenerateWorld(state);

        state->player.chunk = W_GetChunk(state->world, 1, 2, false);
        state->player.pos = (struct Vec2){ 5.0f, 5.0f };
        state->player.vel = (struct Vec2){ 0.0f, 0.0f };
        state->player.rad = (struct Vec2){ 0.35f, 0.2f };
        state->player.tl_point = (struct Vec2){ 0.4f, 2.0f };
        state->player.br_point = (struct Vec2){ 0.4f, 0.0f };
        state->player.animation = CHARACTER_STAND0;
        state->player.render_off = (struct Vec2){ -0.5f, -1.5f };
        W_ChunkAddEntity(state->player.chunk, &state->player);

        state->cam.x = state->player.pos.x;
        state->cam.y = state->player.pos.y;

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

    Move(state->world, &state->player, acc);

    state->cam.x = state->player.pos.x;
    state->cam.y = state->player.pos.y;

    for (struct Entity *ent = state->player.chunk->head; ent != NULL; ent = ent->next) {
        u32 duration = SPRITES[ent->animation].dt;
        if (duration == 1)
            continue;

        ent->render_dt += MS_PER_UPDATE;
        u32 index = SPRITES[ent->animation].index;
        u32 count = SPRITES[ent->animation].count;
        if (duration < ent->render_dt) {
            ent->animation = ent->animation + 1 - ((index + 1 == count) ? count : 0);
            ent->render_dt -= duration;
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

    int screenw, screenh;
    SDL_RenderGetLogicalSize(renderer, &screenw, &screenh);

    if (!state->init) return;

    struct LocalStack render_stack;
    Z_BeginLocalStack(&render_stack, &state->temp_stack);

    if (state->sheets[CHARACTER].w != ~0) {
        int initted = IMG_Init(IMG_INIT_PNG);
        if (initted != IMG_INIT_PNG)
            SDL_LOG("img init failed");

        /* TODO(david): automate this */
        SDL_Surface *temp;
        temp = IMG_Load("../res/sprites/character.png");
        state->sheets[CHARACTER].texture = SDL_CreateTextureFromSurface(renderer, temp);
        state->sheets[CHARACTER].w = ~0;
        SDL_FreeSurface(temp);

        temp = IMG_Load("../res/sprites/tile_wall.png");
        state->sheets[TILE_WALL].texture = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_FreeSurface(temp);
    }

    SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
    SDL_RenderClear(renderer);

    /* TODO(david): render the floor */

    struct RenderLink *first = NULL;
    u32 chunkx = state->player.chunk->x;
    u32 chunky = state->player.chunk->y;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            struct WorldChunk *chunk = W_GetChunk(state->world, chunkx + i, chunky + j, false);
            if (chunk == NULL)
                continue;

            for (struct Entity *ent = chunk->head; ent != NULL; ent = ent->next) {
                struct RenderLink *new = Z_PushStruct(&state->temp_stack, struct RenderLink, true);
                new->ent = ent;
                new->pos = (struct Vec2){ent->pos.x + i * 10.0f, ent->pos.y + j * 10.0f};

                for (struct RenderLink *ren = first; ren != NULL; ren = ren->next) {
                    if (ren->pos.y > new->pos.y) {
                        if (ren == first) {
                            first = new;
                            ren->prev = first;
                        } else {
                            ren->prev->next = new;
                            ren->prev = ren->prev->next;
                        }
                        ren->prev->next = ren;
                        break;
                    } else if (ren->next == NULL) {
                        ren->next = new;
                        new->prev = ren;
                        break;
                    }
                }

                if (first == NULL)
                    first = new;
            }
        }
    }

    SDL_Rect rect;
    for (struct RenderLink *ren = first; ren != NULL; ren = ren->next) {
        struct Entity *ent = ren->ent;
        struct Animation *anim = &SPRITES[ent->animation];

        rect.x = (ren->pos.x + ent->render_off.x - state->cam.x) * PIXEL_PERMETERX + 0.5f + (screenw / 2.0f);
        rect.y = (ren->pos.y + ent->render_off.y - state->cam.y) * PIXEL_PERMETERY + 0.5f + (screenh / 2.0f);
        rect.w = PIXEL_PERMETERX * ((float)(anim->rect.w) / 32.0f); /* TODO(david): not hard coded values */
        rect.h = PIXEL_PERMETERY * ((float)(anim->rect.h) / 24.0f);

        SDL_RenderCopy(renderer, state->sheets[anim->sheet].texture, &anim->rect, &rect);
    }

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

    Z_EndLocalStack(&render_stack);
}

