#ifndef _MAIN_h_
#include "config.h"

/* Consider moving this stuff out to it's own file? */
typedef struct {
    bool was_down;
    u64  half_count;
    u64  last_read;
} GameControl_t;

/**
 * Check if the control was toggled, and set values to ensure toggling
 *
 * @control : whatever the game control is being toggled
 * @return  : true if it was just pressed again
 */
static inline
bool
C_IsToggled(GameControl_t *control)
{
    bool result = control->was_down && (control->last_read != control->half_count);
    control->last_read = result ? control->half_count : control->last_read;
    return result;
}

/**
 * Check if a control is currently being held down
 *
 * @control : the control we want to check
 * @return  : true if the button is currently depressed
 */
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

    /* Holds input for the console or whatever else involves typing...? */
    bool input_entered;
    char input_text[128];
    int  input_len;
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
    ino_t ino; /* TODO(david): this is platform specific */

    void *lib;
    Update_t *Update;
    Render_t *Render;
};

#define _MAIN_h_
#endif
