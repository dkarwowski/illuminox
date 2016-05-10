#ifndef _MAIN_h_
#include <stdbool.h>
#include <stdint.h>

#define CONFIG_PROJ_NAME "proto"
#define CONFIG_SCRN_WIDTH  960
#define CONFIG_SCRN_HEIGHT 560

#define GOAL_FPS 60

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef char        byte;

typedef float       r32;
typedef double      r64;

typedef intptr_t    iptr;
typedef uintptr_t   uptr;

#define SDL_LOG(msg) fprintf(stderr, msg ": %s\n", SDL_GetError())

#define KILOBYTES(x) (x * 1024)
#define MEGABYTES(x) (KILOBYTES(x) * 1024)
#define GIGABYTES(x) (MEGABYTES(x) * 1024)

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
