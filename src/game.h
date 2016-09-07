#ifndef _GAME_h_
#define _GAME_h_

#include <SDL2/SDL_ttf.h>

#include "render_config.h"
#include "math.h"
#include "memory.h"
#include "entity.h"

#define PIXEL_PERMETERX 64
#define PIXEL_PERMETERY 48

enum R_Texture {
    R_player = 0,
    R_floor = 1,
    R_wall = 2,

    R_COUNT
};

struct RenderLink {
    struct Entity     *ent;

    struct RenderLink *next;
    struct RenderLink *prev;

    struct Vec2        pos;
};

#define MAX_ENTITIES 1024
struct GameState {
    bool init;

    struct Stack game_stack;
    struct Stack temp_stack;
    struct WorldState *world;

    TTF_Font *font;

    bool console;
    char buffer[10][128];

    struct Vec2 cam; /* camera to compare to */

    /* player */
    struct Entity player;
    struct Entity ents[MAX_ENTITIES];
    int num_ents;

    /* rendering */
    struct SpriteSheet sheets[SpriteSheet_COUNT];
};

#endif
