
#include "render_config.h"
struct Animation SPRITES[Anim_COUNT] = {
    { .rect={ .x=0, .y=0, .w=32, .h=48 }, .dt=1, .sheet=TILE_WALL,.index=0, .count=1 },
    { .rect={ .x=0, .y=0, .w=32, .h=48 }, .dt=100, .sheet=CHARACTER,.index=0, .count=3 },
    { .rect={ .x=32, .y=0, .w=32, .h=48 }, .dt=100, .sheet=CHARACTER,.index=1, .count=3 },
    { .rect={ .x=64, .y=0, .w=32, .h=48 }, .dt=100, .sheet=CHARACTER,.index=2, .count=3 }
};