#ifndef _MAIN_h_

#define GOAL_FPS 60

#define SDL_LOG(msg) fprintf(stderr, msg ": %s\n", SDL_GetError())

#define KILOBYTES(x) (x * 1024)
#define MEGABYTES(x) (KILOBYTES(x) * 1024)
#define GIGABYTES(x) (MEGABYTES(x) * 1024)

#define SCRN_WIDTH  960
#define SCRN_HEIGHT 560

struct GameInput {
    union {
        struct GameControl control[5];

        struct {
            struct GameControl move_left;
            struct GameControl move_right;
            struct GameControl move_up;
            struct GameControl move_down;
            struct GameControl action;
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
    upd_and_ren_t *update_and_render;
};

#define _MAIN_h_
#endif
