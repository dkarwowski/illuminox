#include <SDL2/SDL.h>

#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <dlfcn.h>

#include "config.h"
#include "main.h"

static
int
HandleEvent( SDL_Event *event,
             struct GameInput *old_input,
             struct GameInput *new_input )
{
    switch (event->type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            bool was_down = event->key.repeat != 0 || event->key.state == SDL_RELEASED;
            bool is_down  = event->key.repeat != 0 || event->key.state == SDL_PRESSED;

            if (was_down != is_down) {
                /* Read over each key defined in config.h */
#define READ_KEY(keycode, control)                                                            \
                if (event->key.keysym.sym == keycode) {                                       \
                    new_input->control.was_down = is_down;                                    \
                    new_input->control.half_count +=                                          \
                        (old_input->control.was_down != new_input->control.was_down) ? 1 : 0; \
                }                                                                             \

                KEY_BINDING(READ_KEY);
#undef READ_KEY
            }
        } break;
        case SDL_QUIT:
        {
            /* allow the game to do any cleanup necessary */
            new_input->quit.was_down = true;
            return -1;
        } break;
        default:
            break;
    }

    return 0;
}

static
int
InitWindowAndRenderer( SDL_Window **window,
                       SDL_Renderer **renderer )
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LOG("Error initializing SDL");
        return -1;
    } else {
        *window = SDL_CreateWindow( CONFIG_PROJ_NAME,
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    CONFIG_SCRN_WIDTH,
                                    CONFIG_SCRN_HEIGHT,
                                    SDL_WINDOW_ALLOW_HIGHDPI );

        if (*window == NULL) {
            SDL_LOG("Error creating window");
            return -2;
        } else {
            *renderer = SDL_CreateRenderer( *window,
                                            -1,
                                            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

            if (*renderer == NULL) {
                SDL_LOG("Error creating renderer");
                return -3;
            }
        }
    }

    return 0;
}

static
int
LoadGame(struct GameLib *game_lib)
{
    struct stat file_stat;
    if (stat("./libgame.so", &file_stat) == 0) {
        /* check whether lib has been updated in last run */
        if (game_lib->ino != file_stat.st_ino) {
            game_lib->ino = file_stat.st_ino;
            game_lib->Update = NULL;
            game_lib->Render = NULL;
        }
    }

    /* only load lib when there isn't anything valid */
    if (game_lib->Update == NULL) {
        if (game_lib->lib) /* ensure we don't have a lib open */
            dlclose(game_lib->lib);

        game_lib->lib = dlopen("./libgame.so", RTLD_LAZY);
        if (!game_lib->lib) {
            fprintf(stderr, "failed to open lib: %s\n", dlerror());
            game_lib->Update = NULL;
            game_lib->Render = NULL;
            return -1;
        } else {
            dlerror(); /* required to clear the error stack */
            game_lib->Update = (Update_t *)dlsym(game_lib->lib, "Update");
            game_lib->Render = (Render_t *)dlsym(game_lib->lib, "Render");
            const char *err = dlerror();
            if (err) {
                fprintf(stderr, "failed to load function: %s\n", err);
                game_lib->Update = NULL;
                game_lib->Render = NULL;
                return -2;
            }
        }
    }

    return 0;
}

static
void
UnloadGame(struct GameLib *game_lib)
{
    if (game_lib->lib)
        dlclose(game_lib->lib);
    game_lib->Update = NULL;
    game_lib->Render = NULL;
}

int
main( int argc,
      char **argv )
{
    SDL_Window *window;
    SDL_Renderer *renderer;

    if (InitWindowAndRenderer(&window, &renderer) == 0) {
        /* preallocate memory to prevent malloc/free usage */
        struct GameMemory memory = { 0 };
        memory.perm_memsize = MEGABYTES(64);
        memory.temp_memsize = MEGABYTES(64);
        memory.perm_mem = mmap( 0, memory.perm_memsize + memory.temp_memsize, PROT_READ | PROT_WRITE,
                                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
        if (memory.perm_mem == MAP_FAILED) {
            fprintf(stderr, "Couldn't create memory map\n");

            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();

            return 2;
        } else {
            memset(memory.perm_mem, 0, memory.perm_memsize + memory.temp_memsize);
            memory.temp_mem = (char *)memory.perm_mem + memory.perm_memsize;

            struct GameInput old_input = { 0 };
            struct GameInput new_input = { 0 };

            /* we want double renderer for better drawing */
            int scrn_w, scrn_h;
            SDL_GetWindowSize(window, &scrn_w, &scrn_h);
            SDL_RenderSetLogicalSize(renderer, scrn_w * 2, scrn_h * 2);

            struct GameLib game_lib  = { 0 };
            LoadGame(&game_lib);

            /* loop variables to keep timing right */
            u64 lag             = 0;
            u64 prev_count      = SDL_GetPerformanceCounter();
            u64 curr_count      = SDL_GetPerformanceCounter();
            const u64 count_ps  = SDL_GetPerformanceFrequency();
            const u64 count_pms = count_ps / 1000;

            bool done = false;
            while (!done) {
                curr_count = SDL_GetPerformanceCounter();
                lag += curr_count - prev_count;
                prev_count = curr_count;

                /* swap input buffers */
                old_input = new_input;

                SDL_Event event;
                while (SDL_PollEvent(&event))
                    done = HandleEvent(&event, &old_input, &new_input);

                /* fixed time step */
                while (lag >= MS_PER_UPDATE*count_pms) {
                    if (game_lib.Update)
                        game_lib.Update(&memory, &new_input);
                    lag -= MS_PER_UPDATE*count_pms;
                }

                /* render, ensure we can update by a fraction of update interval */
                if (game_lib.Render)
                    game_lib.Render(&memory, renderer, (r64)(lag)/(r64)(count_pms));

                if (new_input.quit.was_down)
                    done = true;
            }

            UnloadGame(&game_lib);

            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();

            return 0;
        }
    } else {
        return 1;
    }
}
