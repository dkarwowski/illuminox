#ifndef _GAME_h_
#include "math.h"

#define PIXEL_PERMETERX 64
#define PIXEL_PERMETERY 48

struct Entity {
    u32 worldx, worldy;
    struct Vec2 pos;
    struct Vec2 vel;
    struct Vec2 rad;
};

struct WorldChunk {
    u32 x, y;
    u8 tiles[100];
    struct WorldChunk *next;
};

struct WorldState {
    struct WorldChunk chunks[2048];
};

struct GameState {
    bool init;

    struct WorldState world;

    TTF_Font *font;

    bool console;
    char buffer[10][128];

    /* player */
    struct Entity player;

    SDL_Texture *floor_texture;
    SDL_Texture *wall_texture;
};

#define _GAME_h_
#endif
