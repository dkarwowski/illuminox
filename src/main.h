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

typedef void upd_and_ren_t(struct GameMemory *, struct GameInput *, SDL_Renderer *);

struct GameLib {
    ino_t ino;

    void *lib;
    upd_and_ren_t *UpdateAndRender;
};

#define _MAIN_h_
#endif
