#ifndef _WORLD_h_
#define _WORLD_h_

#include "config.h"
#include "math.h"
#include "memory.h"

struct Entity;
struct GameState;

#define W_CHUNK_DIM (11)

struct WorldChunk {
    u32 x, y;
    struct WorldChunk *next;

    struct Entity *head;
    struct Entity *tail;
};

#define WORLD_HASHSIZE (2048)

struct WorldState {
    struct WorldChunk chunks[WORLD_HASHSIZE];
    struct Stack *stack;
};

struct WorldChunk * W_GetChunk(struct WorldState *world, u32 x, u32 y, bool create);
int                 W_ChunkAddEntity(struct WorldChunk *chunk, struct Entity *ent);
int                 W_ChunkRemoveEntity(struct WorldChunk *chunk, struct Entity *ent);
struct WorldChunk * W_FixChunk(struct WorldState *world, struct WorldChunk *chunk, struct Vec2 *pos);
void                W_GenerateWorld(struct GameState *state);

#endif

