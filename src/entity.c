#include "entity.h"
#include "world.h"

/**
 * Move an entity with a specific acceleration.
 *
 * @world : the current world
 * @ent   : entity that's being moved
 * @acc   : the acceleration we move by
 *
 * The entity should know exactly where it is in the world, as well as its
 * world chunk so that it knows what entities to check.
 */
void
Move(struct WorldState *world, struct Entity *ent, struct Vec2 acc)
{
    ent->vel = V2_Add(V2_Mul(0.95f, ent->vel), V2_Mul(SEC_PER_UPDATE, acc));

    /* don't set the position until after we check collisions */
    struct Vec2 dpos = V2_Mul(SEC_PER_UPDATE, ent->vel);

    /* actual collision detection and handling */
    r32 tleft = 1.0f;
    for (int z = 0; z < 4 && tleft > 0.0f; z++) {
        struct Vec2 normal = {0.0f, 0.0f};
        r32 tmin = 1.0f;
        /* TODO(david): MUST HANDLE OTHER CHUNKS */
        for (struct Entity *cmp_ent = ent->chunk->head; cmp_ent != NULL; cmp_ent = cmp_ent->next) {
            if (cmp_ent == ent)
                continue;

            /* points with small epsilon for flush collision */
            r32 points_y[] = { cmp_ent->pos.x - cmp_ent->rad.x - ent->rad.x - ent->pos.x,
                               cmp_ent->pos.y - cmp_ent->rad.y - ent->rad.y - ent->pos.y,
                               cmp_ent->pos.x + cmp_ent->rad.x + ent->rad.x - ent->pos.x,
                               cmp_ent->pos.y + cmp_ent->rad.y + ent->rad.y - ent->pos.y };
            /* loop over walls defined in this way (top, bottom, left, right) */
            struct {
                r32 x0, x1, y, dy, dx;
                struct Vec2 normal;
            } walls_y[] = {{ points_y[0], points_y[2], points_y[1], dpos.y, dpos.x, {  0.0f, -1.0f } },
                           { points_y[0], points_y[2], points_y[3], dpos.y, dpos.x, {  0.0f,  1.0f } },
                           { points_y[1], points_y[3], points_y[0], dpos.x, dpos.y, { -1.0f,  0.0f } },
                           { points_y[1], points_y[3], points_y[2], dpos.x, dpos.y, {  1.0f,  0.0f } }};

            for (int walli = 0; walli < 4; walli++) {
                r32 epsilon = 0.001f;
                if (fabsf(walls_y[walli].dy) > 0.001f) {
                    r32 t = walls_y[walli].y / walls_y[walli].dy;
                    r32 x = t * walls_y[walli].dx;
                    if (t > 0.0f && walls_y[walli].x0 < x && x < walls_y[walli].x1 && tmin > t) {
                        tmin = MAX(0.0f, t - epsilon);
                        normal = walls_y[walli].normal;
                    }
                }
            }

            /* points with small epsilon for flush collision */
            r32 points_z[] = { cmp_ent->pos.x - cmp_ent->tl_point.x - ent->tl_point.w - ent->pos.x,
                               cmp_ent->pos.y - cmp_ent->tl_point.y - ent->tl_point.h - ent->pos.y,
                               cmp_ent->pos.x + cmp_ent->br_point.x + ent->br_point.w - ent->pos.x,
                               cmp_ent->pos.y + cmp_ent->br_point.y + ent->br_point.h - ent->pos.y };
            /* loop over walls defined in this way (top, bottom, left, right) */
            struct {
                r32 x0, x1, y, dy, dx;
                struct Vec2 normal;
            } walls_z[] = {{ points_z[0], points_z[2], points_z[1], dpos.y, dpos.x, {  0.0f, -1.0f } },
                           { points_z[0], points_z[2], points_z[3], dpos.y, dpos.x, {  0.0f,  1.0f } },
                           { points_z[1], points_z[3], points_z[0], dpos.x, dpos.y, { -1.0f,  0.0f } },
                           { points_z[1], points_z[3], points_z[2], dpos.x, dpos.y, {  1.0f,  0.0f } }};

            for (int walli = 0; walli < 4; walli++) {
                if (fabsf(walls_z[walli].dy) > 0.001f) {
                    r32 t = walls_z[walli].y / walls_z[walli].dy;
                    r32 x = t * walls_z[walli].dx;
                    if (t > 0.0f && walls_z[walli].x0 < x && x < walls_z[walli].x1 && tmin > t) {
                        /* TODO(david): handle a collision */
                    }
                }
            }
        }

        /* adjust old pos with some sort of normal */
        ent->pos = V2_Add(ent->pos, V2_Mul(tmin, dpos));
        ent->vel = V2_Sub(ent->vel, V2_Mul(V2_Dot(ent->vel, normal), normal));
        dpos = V2_Sub(dpos, V2_Mul(V2_Dot(dpos, normal), normal));
        tleft -= tmin;
    }

    struct WorldChunk *new_chunk = W_FixChunk(world, ent->chunk, &ent->pos);
    if (new_chunk != ent->chunk) {
        W_ChunkRemoveEntity(ent->chunk, ent);
        W_ChunkAddEntity(new_chunk, ent);
        ent->chunk = new_chunk;
    }
}

