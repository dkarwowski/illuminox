struct WorldState;
struct WorldChunk;

#include "game.h"
#include "world.h"

/**
 * Get a world chunk from the world, and create one if not found and the 
 * flag is set.
 *
 * @world  : the current world
 * @x      : x coordinate
 * @y      : y coordinate
 * @create : whether or not to create one
 *
 * Create flag should only be set to true when generating the map in the
 * first place.
 *
 * TODO(david): perhaps only use create in a lazy generation?
 */
struct WorldChunk *
W_GetChunk(struct WorldState *world, u32 x, u32 y, bool create)
{
    if (x < 1 || y < 1 || x == ~0 || y == ~0) 
        return NULL;

    u32 hash = (x + y * 31) % WORLD_HASHSIZE;
    struct WorldChunk *result = &world->chunks[hash];
    while (result != NULL) {
        if (result->next == NULL && (result->x != x || result->y != y) && create) {
            /* TODO(david): need proper alloc */
            result->next = Z_PushStruct(&world->stack, struct WorldChunk, true);
            result = result->next;
            result->x = x;
            result->y = y;
        } 
        if (result->x == x && result->y == y) {
            return result;
        }
        
        result = result->next;
    }

    return NULL;
}

/**
 * Add an entity to a chunk
 *
 * @chunk  : what to add to
 * @ent    : entity to add
 * @return : 0 on success, else error val if there is one
 */
int
W_ChunkAddEntity(struct WorldChunk *chunk, struct Entity *ent)
{
    if (!chunk)
        return 1;
    if (!ent)
        return 2;

    if (chunk->head == NULL) {
        chunk->head = ent;
        chunk->tail = ent;
    }
    else {
        chunk->tail->next = ent;
    }

    /* we're adding to the end, so this should be NULL */
    ent->next = NULL;
    ent->prev = chunk->tail;
    chunk->tail = ent;

    return 0;
}

/**
 * Remove an entity from achunk
 *
 * @chunk  : the chunk to remove from
 * @ent    : entity to remove
 * @return : 0 on success, else some error value
 */
int
W_ChunkRemoveEntity(struct WorldChunk *chunk, struct Entity *ent)
{
    if (!chunk)
        return 1;
    if (!ent)
        return 2;

    if (ent == chunk->head)
        chunk->head = ent->next;
    if (ent == chunk->tail)
        chunk->tail = ent->prev;

    if (ent->prev)
        ent->prev->next = ent->next;
    if (ent->next)
        ent->next->prev = ent->prev;

    return 0;
}

/**
 * Adjust the position and get the correct chunk for the position when moved out
 * of the current bounds, and optionally create the chunk if it doesn't already
 * exist
 *
 * @world  : world containing chunks
 * @chunk  : original chunk
 * @pos    : the new position
 * @return : the proper chunk
 */
struct WorldChunk *
W_FixChunkCreate(struct WorldState *world, struct WorldChunk *chunk, struct Vec2 *pos, bool create)
{
    u32 world_e[] = { chunk->x, chunk->y };
    for (int i = 0; i < 2; i++) {
        if (pos->e[i] < 0.0f) {
            world_e[i] -= 1;
            pos->e[i] += 10.0f;
        } else if (pos->e[i] >= 10.0f) {
            world_e[i] += 1;
            pos->e[i] -= 10.0f;
        }
    }

    if (world_e[0] != chunk->x || world_e[1] != chunk->y) {
        return W_GetChunk(world, world_e[0], world_e[1], create);
    }
    return chunk;
}

/**
 * Adjust the position and get the correct chunk for the position when moved out
 * of the current bounds.
 *
 * @world : the world containing chunks
 * @chunk : the original chunk
 * @pos   : position that may be off
 */
struct WorldChunk *
W_FixChunk(struct WorldState *world, struct WorldChunk *chunk, struct Vec2 *pos)
{
    return W_FixChunkCreate(world, chunk, pos, false);
}

/**
 * Generate a world for the given state
 *
 * @state : game state struct where we add the world
 */
void
W_GenerateWorld(struct GameState *state)
{
    struct WorldChunk *chunk = W_GetChunk(state->world, 1, 1, true);

    struct LocalStack lstack;
    Z_BeginLocalStack(&lstack, &state->temp_stack);

    /* ensure first is null when starting */
    chunk->head = NULL;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (j == 0 || j == 9 || i == 0 || (i == 9 && j != 4 && j != 5)) {
                state->ents[state->num_ents] = (struct Entity) { .chunk = chunk,
                                                                 .pos.x = (r32)j + 0.5f,
                                                                 .pos.y = (r32)i + 0.5f,
                                                                 .vel = (struct Vec2){ 0.0f, 0.0f },
                                                                 .rad = (struct Vec2){ 0.5f, 0.5f },
                                                                 .tl_point = (struct Vec2){ 0.0f, 0.0f },
                                                                 .br_point = (struct Vec2){ 0.0f, 0.0f },
                                                                 .animation = TILE_WALL_STAND0,
                                                                 .render_off = (struct Vec2){ -0.5f, -1.5f } };

                W_ChunkAddEntity(chunk, &state->ents[state->num_ents]);

                state->num_ents++;
            }
        }
    }
    chunk = W_GetChunk(state->world, 1, 2, true);
    chunk->head = NULL;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (j == 0 || j == 9 || i == 9 || (i == 0 && j != 4 && j != 5)) {
                state->ents[state->num_ents] = (struct Entity) { .chunk = chunk,
                                                                 .pos.x = (r32)j + 0.5f,
                                                                 .pos.y = (r32)i + 0.5f,
                                                                 .vel = (struct Vec2){ 0.0f, 0.0f },
                                                                 .rad = (struct Vec2){ 0.5f, 0.5f },
                                                                 .tl_point = (struct Vec2){ 0.0f, 0.0f },
                                                                 .br_point = (struct Vec2){ 0.0f, 0.0f },
                                                                 .animation = TILE_WALL_STAND0,
                                                                 .render_off = (struct Vec2){ -0.5f, -1.5f } };

                W_ChunkAddEntity(chunk, &state->ents[state->num_ents]);

                state->num_ents++;
            }
        }
    }

    Z_EndLocalStack(&lstack);
}
