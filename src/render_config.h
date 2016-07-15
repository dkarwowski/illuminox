#ifndef _GAME_CONFIG_h_
#define _GAME_CONFIG_h_

#include <SDL2/SDL.h>
#include "config.h"

/* MAKE AUTOGEN - DO NOT CHANGE ANYTHING UNDER THIS LINE */

/* all sprite sheets */
enum SpriteSheetId {
    TILE_WALL,
    CHARACTER,
    SpriteSheet_COUNT
};

/* sprite sheet struct */
struct SpriteSheet {
    SDL_Texture *texture;
    i32 w, h;
};

/* tag mapped to animation */
enum AnimationId {
    TILE_WALL_STAND0,
    CHARACTER_STAND0,
    CHARACTER_STAND1,
    CHARACTER_STAND2,
    Anim_COUNT
};

/* struct for animations */
struct Animation {
    SDL_Rect rect;
    enum SpriteSheetId sheet;
    u32 dt;
    u32 index;
    u8 count;
};

/* animations list */
extern struct Animation SPRITES[Anim_COUNT];

#endif