#ifndef _CONFIG_h_
#define _CONFIG_h_

#include <stddef.h>
#include <stdint.h>

/* ensure there's only one of each bound name */
#define KEY_BINDING(_def) \
    _def(SDLK_BACKQUOTE, console); \
    _def(SDLK_a, move_left); \
    _def(SDLK_d, move_right); \
    _def(SDLK_w, move_up); \
    _def(SDLK_s, move_down); \
    _def(SDLK_SPACE, action); \
    _def(SDLK_ESCAPE, quit); \

#define CONFIG_PROJ_NAME   "proto"
#define CONFIG_SCRN_WIDTH  1280
#define CONFIG_SCRN_HEIGHT 720

/* runs at 120 FPS like this */
#define MS_PER_UPDATE 8
#define SEC_PER_UPDATE (1.0f/1000.0f * MS_PER_UPDATE)
#define GOAL_FPS 60

/* define true and false stuff here, no stdbool */
#define true          1
#define false         0
#define bool          _Bool

/* typedef these to be least to ensure they exist */
typedef uint_least8_t  u8;
typedef uint_least16_t u16;
typedef uint_least32_t u32;
typedef uint_least64_t u64;

typedef int_least8_t   i8;
typedef int_least16_t  i16;
typedef int_least32_t  i32;
typedef int_least64_t  i64;

typedef unsigned char  byte;

typedef float          r32;
typedef double         r64;

typedef intptr_t       iptr;
typedef uintptr_t      uptr;

#define SDL_LOG(msg) fprintf(stderr, msg ": %s\n", SDL_GetError())

#define KILOBYTES(x) (x * 1024)
#define MEGABYTES(x) (KILOBYTES(x) * 1024)
#define GIGABYTES(x) (MEGABYTES(x) * 1024)

#ifdef DEBUG
#include <assert.h>
#define ASSERT(expr) assert(expr)
#else
#define ASSERT(expr) void
#endif

#endif
