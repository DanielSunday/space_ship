#include "engine_data.h"

#include <stdlib.h>
#include <stdio.h>

#define ENGINE_DATA_ADDR "assets/engine_data.txt"

engine_data_t e;

static v2f_t scan_v2f(FILE *f)
{
    v2f_t v;
    (void) fscanf(f, "%f%f", &v.x, &v.y);
    return v;
}

static char *scan_quoted_str(FILE *f)
{
    char *str;
    int c, len = 0;
    for (c = fgetc(f); c != '"'; c = fgetc(f));       /* 1st quote behind */
    for (c = fgetc(f); c != '"'; c = fgetc(f)) ++len; /* 2nd quote behind */
    str = malloc(len + 1);
    str[len] = '\0';
    fseek(f, -len - 1, SEEK_CUR);                /* 1st quote  behind  */
    for (c = 0; c < len; ++c) str[c] = fgetc(f); /* 2nd quote in front */
    (void) fgetc(f);                             /* 2nd quote  behind  */
    return str;
}

static void init_hero(FILE *f, hero_data_t *hero)
{
    hero->pos = scan_v2f(f);
    hero->vel = scan_v2f(f);
    (void) fscanf(f, "%f%f%f", &hero->rot, &hero->rot_l, &hero->rot_r);
    (void) fscanf(f, "%f%f%f", &hero->bar_h, &hero->usage, &hero->thrust);
}

static void init_dest(FILE *f, dest_data_t *dest)
{
    dest->pos = scan_v2f(f);
    (void) fscanf(f, "%f", &dest->r);
}

static void init_bmp(FILE *f, bool_map_t *bmp)
{
    int tile_size, x, y, w, h;
    (void) fscanf(f, "%d", &tile_size);
    w = bmp->w = e.win_w / tile_size;
    h = bmp->h = e.win_h / tile_size;
    bmp->m = malloc(w * h * sizeof(*bmp->m));
    for(y=h-1;y>=0;--y)for(x=0;x<w;++x)(void)fscanf(f,"%d",(int*)&bmp->m[y*w+x]);
}

static void init_level(FILE *f, level_data_t *level)
{
    int j;
    for (j = 0; j < NO_ADDRS; ++j) level->addrs[j] = scan_quoted_str(f);
    init_hero(f, &level->hero);
    init_dest(f, &level->dest);
    level->grav = scan_v2f(f);
    level->term = scan_v2f(f);
    init_bmp(f, &level->bmp);
}

void init_engine_data(void)
{
    int i;
    FILE *f;
    f = fopen(ENGINE_DATA_ADDR, "r");
    (void)fscanf(f,"%d%d%d%d",&e.key_up, &e.key_left, &e.key_right, &e.key_esc);
    (void)fscanf(f,"%d%d%d%d",&e.no_levels,&e.win_w,&e.win_h,(int*)&e.win_fscr);
    e.levels = malloc(e.no_levels * sizeof(*e.levels));
    for (i = 0; i < e.no_levels; ++i) init_level(f, &e.levels[i]);
    fclose(f);
}

static void exit_level(level_data_t *level)
{
    int i;
    for (i = 0; i < NO_ADDRS; ++i) free(level->addrs[i]);
    free(level->bmp.m);
}

void exit_engine_data(void)
{
    for (e.i = 0; e.i < e.no_levels; ++e.i) exit_level(&e.levels[e.i]);
    free(e.levels);
}
