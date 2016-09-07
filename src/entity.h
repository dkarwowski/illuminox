#ifndef _ENTITY_h_
#define _ENTITY_h_

struct WorldChunk;
struct WorldState;

#include "config.h"
#include "math.h"
#include "render_config.h"

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

void Move(struct WorldState *world, struct Entity *ent, struct Vec2 acc);

#endif
