#ifndef ENGINE_DATA_H
#define ENGINE_DATA_H

#include "bool.h"

typedef struct v2f_t
{
    float x, y;
}
v2f_t;

typedef enum ADDRS
{
    ADDR_VERT,
    ADDR_FRAG,
    ADDR_BG,
    ADDR_FG,
    ADDR_SOUND,
    NO_ADDRS
}
ADDRS;

typedef struct hero_data_t
{
    v2f_t pos, vel; /* Position, velocity */
    float rot, rot_l, rot_r, bar_h, usage, thrust;
}
hero_data_t;

typedef struct dest_data_t
{
    v2f_t pos;
    float r;
}
dest_data_t;

typedef struct bool_map_t
{
    int w, h;
    bool *m;
}
bool_map_t;

typedef struct level_data_t
{
    char *addrs[NO_ADDRS];
    hero_data_t hero;
    dest_data_t dest;
    v2f_t grav, term; /* Gravity acceleration, terminal velocity */
    bool_map_t   bmp;
}
level_data_t;

typedef struct engine_data_t
{
    int key_up, key_left, key_right, key_esc, no_levels, i, win_w, win_h;
    bool win_fscr;
    level_data_t *levels;
}
engine_data_t;

extern engine_data_t e;

void init_engine_data(void);
void exit_engine_data(void);

#endif
