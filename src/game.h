#ifndef _GAME_h_

#define PIXEL_PERMETER 100

struct GameState {
    bool init;

    u8 tiles[100];

    TTF_Font *font;

    bool console;
    char buffer[10][128];
};

#define _GAME_h_
#endif
