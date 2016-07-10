#ifndef _GAME_h_
#include "math.h"
#include "game_config.h"

#define PIXEL_PERMETERX 64
#define PIXEL_PERMETERY 48

enum R_Texture {
    R_player = 0,
    R_floor = 1,
    R_wall = 2,

    R_COUNT
};

struct Entity {
    struct WorldChunk  *chunk;

    struct Entity      *prev;
    struct Entity      *next;

    struct Vec2        pos;
    struct Vec2        vel;
    struct Vec2        rad; /* floor radius */

    /* offset from pos to top left, then bot right */
    struct Vec2        tl_point;
    struct Vec2        br_point;

    enum   AnimationId animation;
    struct Vec2        render_off;
    u32                render_dt;
};

struct RenderLink {
    struct Entity     *ent;

    struct RenderLink *next;
    struct RenderLink *prev;

    struct Vec2        pos;
};

struct WorldChunk {
    u32 x, y;
    struct WorldChunk *next;

    struct Entity *head;
    struct Entity *tail;
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

    struct Vec2 cam; /* camera to compare to */

    /* player */
    struct Entity player;
    struct Entity ents[256];
    int num_ents;

    /* rendering */
    struct SpriteSheet sheets[SpriteSheet_COUNT];
};

#define _GAME_h_
#endif
