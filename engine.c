#include "engine.h"
#include "engine_data.h"
#include "engine_renderer.h"
#include <math.h>
#include "sound.h"

#define PI 3.14159265359

typedef struct engine_logic_data_t
{
    v2f_t pos, vel;
    float fuel, rot, last_time;
    bool thrust, open, loop, play;
}
engine_logic_data_t;

static engine_logic_data_t el;

void init_engine(void)
{
    init_engine_data();
    init_engine_renderer();
    init_sound_system();
    el.open = el.loop = el.play = true;
}

void exit_engine(void)
{
    exit_sound_system();
    exit_engine_renderer();
    exit_engine_data();
}

static void change_position(void)
{
    GLFWwindow *win;
    float curr_time, elapsed;

    win = engine_renderer_window();

    /* Updating internal timer */
    curr_time = glfwGetTime();
    elapsed = curr_time - el.last_time;
    el.last_time = curr_time;

    /* Rotating the space ship */
    if (glfwGetKey(win, e.key_left))  el.rot += e.levels[e.i].hero.rot_l * elapsed;
    if (glfwGetKey(win, e.key_right)) el.rot -= e.levels[e.i].hero.rot_r * elapsed;

    /* Burning fuel */
    if (el.fuel > 0 && glfwGetKey(win, e.key_up))
    {
        if (el.thrust == false)
        {
            el.thrust = true;
            play_sound();
        }
        el.fuel -= e.levels[e.i].hero.usage * elapsed;
        if (el.fuel < 0) el.fuel = 0;
        el.vel.x += cos(el.rot + 0.5 * PI) * e.levels[e.i].hero.thrust * elapsed;
        el.vel.y += sin(el.rot + 0.5 * PI) * e.levels[e.i].hero.thrust * elapsed;
    }
    else if (el.thrust == true)
    {
        el.thrust = false;
        stop_sound();
    }

    /* Applying gravity acceleration and terminal velocity */
    el.vel.x += elapsed * e.levels[e.i].grav.x;
    el.vel.y += elapsed * e.levels[e.i].grav.y;
    if (el.vel.x > e.levels[e.i].term.x) el.vel.x =  e.levels[e.i].term.x;
    if (el.vel.y > e.levels[e.i].term.y) el.vel.y =  e.levels[e.i].term.y;
    if (el.vel.x <-e.levels[e.i].term.y) el.vel.x = -e.levels[e.i].term.x;
    if (el.vel.y <-e.levels[e.i].term.y) el.vel.y = -e.levels[e.i].term.y;

    /* Applying the space ship velocity to it's position */
    el.pos.x += el.vel.x;
    el.pos.y += el.vel.y;
}

static bool hero_finished(void)
{
    dest_data_t *dest;
    float x, y;

    dest = &e.levels[e.i].dest;
    x = (dest->pos.x + dest->r) - (el.pos.x + 0.5);
    y = (dest->pos.y + dest->r) - (el.pos.y + 0.5);
    return  (x * x + y * y < (dest->r - 0.5) * (dest->r - 0.5));
}

static bool hero_went_outside_level_bounds(void)
{
    return el.pos.x < 0 || el.pos.y < 0 || el.pos.x >= e.levels[e.i].bmp.w - 1 || el.pos.y >= e.levels[e.i].bmp.h - 1;
}

static bool hero_collided(void)
{
    int sw, se, nw, ne, x, y, w;
    x = el.pos.x;
    y = el.pos.y;
    w = e.levels[e.i].bmp.w;
    sw = y * w + x;
    se = sw + 1;
    nw = sw + w;
    ne = nw + 1;
    if (e.levels[e.i].bmp.m[sw] && (el.pos.x - x) * (el.pos.x - x) + (el.pos.y - y) * (el.pos.y - y) < 1) return true;
    if (e.levels[e.i].bmp.m[se] && (el.pos.x - x - 1) * (el.pos.x - x - 1) + (el.pos.y - y) * (el.pos.y - y) < 1) return true;
    if (e.levels[e.i].bmp.m[nw] && (el.pos.x - x) * (el.pos.x - x) + (el.pos.y - y - 1) * (el.pos.y - y - 1) < 1) return true;
    if (e.levels[e.i].bmp.m[ne] && (el.pos.x - x - 1) * (el.pos.x - x - 1) + (el.pos.y - y - 1) * (el.pos.y - y - 1) < 1) return true;
    return false;
}

static void change_program_flow(void)
{
    glfwPollEvents();
    if (glfwWindowShouldClose(engine_renderer_window())
    ||  glfwGetKey(engine_renderer_window(), e.key_esc)) el.open = el.loop = el.play = false;
    if (hero_finished()) el.loop = el.play = false;
    if (hero_went_outside_level_bounds() || hero_collided()) el.play = false;
}

void run_engine(int i)
{
    if (el.open)
    {
        e.i = i;
        el.loop = true;
        init_engine_renderer_level();
        init_sound(e.levels[e.i].addrs[ADDR_SOUND]);
        while (el.loop)
        {
            el.play = true;
            el.pos = e.levels[i].hero.pos;
            el.vel = e.levels[i].hero.vel;
            el.rot = e.levels[i].hero.rot;
            el.fuel = 1;
            el.last_time = 0;
            glfwSetTime(0);
            while (el.play)
            {
                change_position();
                draw_engine_renderer_level(el.pos, sin(el.rot), cos(el.rot), el.fuel, el.thrust);
                change_program_flow();
            }
        }
        exit_sound();
        exit_engine_renderer_level();
    }
}

int get_engine_no_levels(void)
{
    return e.no_levels;
}
