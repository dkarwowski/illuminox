#ifndef _MAIN_h_
#include <stdbool.h>
#include <stdint.h>
#include "config.h"

typedef struct {
    bool was_down;
    u64  half_count;
    u64  last_read;
} GameControl_t;

static inline
bool
C_IsToggled(GameControl_t *control)
{
    bool result = control->was_down && (control->last_read != control->half_count);
    control->last_read = result ? control->half_count : control->last_read;
    return result;
}

static inline
bool
C_IsPressed(GameControl_t *control)
{
    return control->was_down;
}

struct GameInput {
    union {
        GameControl_t control[5];

        struct {
            GameControl_t move_left;
            GameControl_t move_right;
            GameControl_t move_up;
            GameControl_t move_down;
            GameControl_t action;

            GameControl_t console;

            GameControl_t quit;
        };
    };
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
