#ifndef _GAME_h_
#include "math.h"

#define PIXEL_PERMETERX 64
#define PIXEL_PERMETERY 48

struct Entity {
    struct WorldChunk *chunk;

    struct Entity *prev;
    struct Entity *next;

    struct Vec2 pos;
    struct Vec2 vel;
    struct Vec2 rad;
};

struct RenderLink {
    struct Entity *ent;
    struct RenderLink *next;
    struct RenderLink *prev;
};

struct WorldChunk {
    u32 x, y;
    struct WorldChunk *next;

    struct Entity *first;
};

#define WORLD_HASHSIZE 2048
struct WorldState {
    struct WorldChunk chunks[WORLD_HASHSIZE];
};

struct GameState {
    bool init;

    struct WorldState world;

    TTF_Font *font;

    bool console;
    char buffer[10][128];

    /* player */
    struct Entity player;
    struct Entity ents[256];
    int num_ents;

    SDL_Texture *floor_texture;
    SDL_Texture *wall_texture;
};

#define _GAME_h_
#endif
