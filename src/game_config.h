#ifndef _GAME_CONFIG_h_

/* MAKE AUTOGEN - DO NOT CHANGE ANYTHING UNDER THIS LINE */

/* all sprite sheets */
enum SpriteSheetId {
    CHARACTER,
    SpriteSheet_COUNT
};

/* sprite sheet struct */
struct SpriteSheet {
    SDL_Texture *texture;
    i32 w, h;
};

/* list of sprite sheets */
struct SpriteSheet sheets[SpriteSheet_COUNT];

/* tag mapped to animation */
enum AnimationId {
    CHARACTER_STAND0,
    CHARACTER_STAND1,
    CHARACTER_STAND2,
    Anim_COUNT
};

/* struct for animations */
struct Animation {
    SDL_Rect rect;
    u32 dt;
    u32 index;
    u8 count;
};

/* animations list */
struct Animation SPRITES[Anim_COUNT] = {
    { .rect={ .x=0, .y=0, .w=32, .h=48 }, .dt=100, .index=0, .count=3 },
    { .rect={ .x=32, .y=0, .w=32, .h=48 }, .dt=100, .index=1, .count=3 },
    { .rect={ .x=64, .y=0, .w=32, .h=48 }, .dt=100, .index=2, .count=3 }
};

#define _GAME_CONFIG_h_
#endif