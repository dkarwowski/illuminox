#ifndef _GAME_h_
#include "math.h"

#define PIXEL_PERMETER 100

struct GameState {
    bool init;

    u8 tiles[100];

    TTF_Font *font;

    bool console;
    char buffer[10][128];

    /* player position */
    struct Vec2 pos;
    struct Vec2 vel;
};

#define _GAME_h_
#endif
