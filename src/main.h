#ifndef _MAIN_h_
#include <stdbool.h>
#include <stdint.h>
#include "config.h"

struct GameControl {
    u64 was_down;
    u64 half_count;
};

struct GameInput {
    union {
        struct GameControl control[5];

        struct {
            struct GameControl move_left;
            struct GameControl move_right;
            struct GameControl move_up;
            struct GameControl move_down;
            struct GameControl action;

            struct GameControl quit;
        };
    };

    r64 dt;
};

struct GameMemory {
    bool is_init;

    u64 perm_memsize;
    void *perm_mem;
    u64 temp_memsize;
    void *temp_mem;
};

#define UPDATE(name) void name(struct GameMemory *memory, struct GameInput *input)
typedef UPDATE(Update_t);

#define RENDER(name) void name(struct GameMemory *memory, SDL_Renderer *renderer, r64 dt)
typedef RENDER(Render_t);

struct GameLib {
    ino_t ino;

    void *lib;
    Update_t *Update;
    Render_t *Render;
};

#define _MAIN_h_
#endif
