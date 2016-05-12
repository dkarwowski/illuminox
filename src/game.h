#ifndef _GAME_h_

#define PIXEL_PERMETER 100

struct GameState {
    bool init;
    bool console;

    u8 tiles[100];

    TTF_Font *font;
};

#define _GAME_h_
#endif
