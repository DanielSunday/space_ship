#include "engine_renderer.h"
#include "gutil.h"
#include <stdlib.h>

#define WIN_TITLE "Space Ship"
#define INDEX_T GLubyte
#define INDEX_MACRO GL_UNSIGNED_BYTE

typedef enum VARS
{
    VAR_TEX,
    VAR_SCL,
    VAR_ROT,
    VAR_MOV,
    NO_VARS
}
VARS;

typedef struct drawable_t
{
    GLuint no_indices, indices, scrpts, texpts, tex;
    v2f_t scl, rot, mov;
}
drawable_t;

typedef struct engine_renderer_data_t
{
    GLFWwindow *win;
    GLuint vao, prg, bg_tex, fg_tex;
    GLint locs[NO_VARS];
    drawable_t bg, fg, bar, hero, flame;
}
engine_renderer_data_t;

static engine_renderer_data_t er;

void init_engine_renderer(void)
{
    (void) glfwInit();
    er.win = mk_win(e.win_w, e.win_h, WIN_TITLE, e.win_fscr);
    er.vao = mk_vao(2);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void exit_engine_renderer(void)
{
    glDisable(GL_BLEND);
    er.vao = rm_vao(er.vao, 2);
    er.win = rm_win(er.win);
    glfwTerminate();
}

static void i_bg(void)
{
    const INDEX_T      is[] = {0, 1, 2, 2, 3, 0};
    const v2f_t    scrpts[] = {{-1, -1}, {1, -1}, {1, 1}, {-1, 1}};
    const v2f_t    texpts[] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    er.bg.no_indices = 6;
    er.bg.tex = er.bg_tex;
    er.bg.scl.x = er.bg.scl.y = 1;
    er.bg.mov.x = er.bg.mov.y = 0;
    er.bg.rot.x = 0; /* sin(0) = 0 */
    er.bg.rot.y = 1; /* cos(0) = 1 */
    er.bg.indices= mk_vbo(GL_ELEMENT_ARRAY_BUFFER,sizeof(is),is,GL_STATIC_DRAW);
    er.bg.scrpts = mk_vbo(GL_ARRAY_BUFFER,sizeof(scrpts),scrpts,GL_STATIC_DRAW);
    er.bg.texpts = mk_vbo(GL_ARRAY_BUFFER,sizeof(texpts),texpts,GL_STATIC_DRAW);
}

static void add_tile(int x, int y, int w, const bool *m, GLuint *no_indices,
                     INDEX_T **indices, v2f_t **scrpts, v2f_t **texpts)
{
    static const INDEX_T base_indices[] = {0, 1, 2, 2, 3, 0};
    static const v2f_t base_scrpts[] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    static const v2f_t base_texpts[] = {{0.5, 0}, {1, 0}, {1, 0.5}, {0.5, 0.5}};
    int no_pts, i;
    if (m[y * w + x])
    {
        *no_indices += 6;
        no_pts = *no_indices / 6 * 4;
        *indices = realloc(*indices, *no_indices * sizeof(**indices));
        *scrpts = realloc(*scrpts, no_pts * sizeof(**scrpts));
        *texpts = realloc(*texpts, no_pts * sizeof(**texpts));
        for (i=0;i<6;++i) (*indices)[i+*no_indices-6]=base_indices[i]+no_pts-4;
        for (i=0;i<4;++i)
        {
            (*scrpts)[i + no_pts - 4].x = base_scrpts[i].x + x;
            (*scrpts)[i + no_pts - 4].y = base_scrpts[i].y + y;
            (*texpts)[i + no_pts - 4] = base_texpts[i];
        }
    }
}

static void add_dest(GLuint*no_indices,INDEX_T**is,v2f_t**scrpts,v2f_t**texpts)
{ /* this functions seems redundant, but whatever... */
    const INDEX_T base_indices[6] = {0, 1, 2, 2, 3, 0};
    const v2f_t   base_texpts[4] = {{0, 0.5}, {0.5, 0.5}, {0.5, 1}, {0, 1}};
    v2f_t         dest_scrpts[4];
    int no_pts, i;
    for (i = 0; i < 4; ++i) dest_scrpts[i] = e.levels[e.i].dest.pos;
    dest_scrpts[1].x += e.levels[e.i].dest.r * 2;
    dest_scrpts[2].x += e.levels[e.i].dest.r * 2;
    dest_scrpts[2].y += e.levels[e.i].dest.r * 2;
    dest_scrpts[3].y += e.levels[e.i].dest.r * 2;
    *no_indices += 6;
    no_pts = *no_indices / 6 * 4;
    *is = realloc(*is, *no_indices * sizeof(**is));
    *scrpts = realloc(*scrpts, no_pts * sizeof(**scrpts));
    *texpts = realloc(*texpts, no_pts * sizeof(**texpts));
    for (i=0;i<6;++i) (*is)[i+*no_indices-6]=base_indices[i]+no_pts-4;
    for (i=0;i<4;++i) (*scrpts)[i + no_pts - 4] = dest_scrpts[i];
    for (i=0;i<4;++i) (*texpts)[i + no_pts - 4] = base_texpts[i];
}

static void i_fg(void)
{
    size_t sz_is, sz_pts;
    int x, y, w, h;
    const bool *m;
    INDEX_T *is = NULL;
    v2f_t *scrpts = NULL;
    v2f_t *texpts = NULL;
    w = e.levels[e.i].bmp.w;
    h = e.levels[e.i].bmp.h;
    m = e.levels[e.i].bmp.m;
    er.fg.no_indices = 0;
    er.fg.tex = er.fg_tex;
    er.fg.scl.x = 2.0f / (float) w;
    er.fg.scl.y = 2.0f / (float) h;
    er.fg.rot.x = 0; /* sin(0) = 0 */
    er.fg.rot.y = 1; /* cos(0) = 1 */
    er.fg.mov.x = er.fg.mov.y = -1;
    for (y = 0; y < h; ++y)
        for (x = 0; x < w; ++x)
            add_tile(x, y, w, m, &er.fg.no_indices, &is, &scrpts, &texpts);
    add_dest(&er.fg.no_indices, &is, &scrpts, &texpts);
    sz_is = er.fg.no_indices * sizeof(*is);
    sz_pts= er.fg.no_indices / 6 * 4 * sizeof(*scrpts);
    er.fg.indices= mk_vbo(GL_ELEMENT_ARRAY_BUFFER, sz_is, is, GL_STATIC_DRAW);
    er.fg.scrpts = mk_vbo(GL_ARRAY_BUFFER, sz_pts, scrpts, GL_STATIC_DRAW);
    er.fg.texpts = mk_vbo(GL_ARRAY_BUFFER, sz_pts, texpts, GL_STATIC_DRAW);
    free(texpts);
    free(scrpts);
    free(is);
}

static void i_bar(void)
{
    const INDEX_T      is[] = {0, 1, 2, 2, 3, 0};
          v2f_t    scrpts[] = {{-1, -1}, {1, -1}, {1, 1}, {-1, 1}};
    const v2f_t    texpts[] = {{0.25, 0}, {0.5, 0}, {0.5, 0.5}, {0.25, 0.5}};
    scrpts[2].y = -1+2*e.levels[e.i].hero.bar_h / e.levels[e.i].bmp.h;
    scrpts[3].y = scrpts[2].y;
    er.bar.no_indices = 6;
    er.bar.tex = er.fg_tex;
    er.bar.scl.x = er.bar.scl.y = 1;
    er.bar.mov.x = er.bar.mov.y = 0;
    er.bar.rot.x = 0; /* sin(0) = 0 */
    er.bar.rot.y = 1; /* cos(0) = 1 */
    er.bar.indices= mk_vbo(GL_ELEMENT_ARRAY_BUFFER,sizeof(is),is,GL_STATIC_DRAW);
    er.bar.scrpts = mk_vbo(GL_ARRAY_BUFFER,sizeof(scrpts),scrpts,GL_STATIC_DRAW);
    er.bar.texpts = mk_vbo(GL_ARRAY_BUFFER,sizeof(texpts),texpts,GL_STATIC_DRAW);
}

static void i_hero(void)
{
    const INDEX_T      is[] = {0, 1, 2, 2, 3, 0};
          v2f_t    scrpts[] = {{-1, -1}, {1, -1}, {1, 1}, {-1, 1}};
    const v2f_t    texpts[] = {{0.5, 0.5}, {1, 0.5}, {1, 1}, {0.5, 1}};
    er.hero.no_indices = 6;
    er.hero.tex = er.fg_tex;
    er.hero.scl.x = 1.0f / e.levels[e.i].bmp.w;
    er.hero.scl.y = 1.0f / e.levels[e.i].bmp.h;
    er.hero.mov.x = er.hero.mov.y = 0;
    er.hero.rot.x = 0; /* sin(0) = 0 */
    er.hero.rot.y = 1; /* cos(0) = 1 */
    er.hero.indices= mk_vbo(GL_ELEMENT_ARRAY_BUFFER,sizeof(is),is,GL_STATIC_DRAW);
    er.hero.scrpts = mk_vbo(GL_ARRAY_BUFFER,sizeof(scrpts),scrpts,GL_STATIC_DRAW);
    er.hero.texpts = mk_vbo(GL_ARRAY_BUFFER,sizeof(texpts),texpts,GL_STATIC_DRAW);
}

static void i_flame(void)
{
    const INDEX_T      is[] = {0, 1, 2, 2, 3, 0};
          v2f_t    scrpts[] = {{-0.5, -3}, {0.5, -3}, {0.5, -1}, {-0.5, -1}};
    const v2f_t    texpts[] = {{0, 0}, {0.25, 0}, {0.25, 0.5}, {0, 0.5}};
    er.flame.no_indices = 6;
    er.flame.tex = er.fg_tex;
    er.flame.mov.x = er.flame.mov.y = 0;
    er.flame.rot.x = 0; /* sin(0) = 0 */
    er.flame.rot.y = 1; /* cos(0) = 1 */
    er.flame.indices= mk_vbo(GL_ELEMENT_ARRAY_BUFFER,sizeof(is),is,GL_STATIC_DRAW);
    er.flame.scrpts = mk_vbo(GL_ARRAY_BUFFER,sizeof(scrpts),scrpts,GL_STATIC_DRAW);
    er.flame.texpts = mk_vbo(GL_ARRAY_BUFFER,sizeof(texpts),texpts,GL_STATIC_DRAW);
}

void init_engine_renderer_level(void)
{
    level_data_t *l;
    const char *n[] = {"tex", "scl", "rot", "mov"};
    l = &e.levels[e.i];
    er.prg = mk_prg(l->addrs[ADDR_VERT],l->addrs[ADDR_FRAG],NO_VARS,n,er.locs);
    glClearColor(0, 0, 0, 1);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(er.locs[VAR_TEX], 0);
    er.bg_tex = mk_tex(l->addrs[ADDR_BG]);
    er.fg_tex = mk_tex(l->addrs[ADDR_FG]);
    i_bg(); i_fg(); i_bar(); i_hero(); i_flame();
}

static void e_drw(drawable_t *drw)
{
    drw->texpts = rm_vbo(drw->texpts);
    drw->scrpts = rm_vbo(drw->scrpts);
    drw->indices = rm_vbo(drw->indices);
}

void exit_engine_renderer_level(void)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    e_drw(&er.flame);e_drw(&er.hero);e_drw(&er.bar);e_drw(&er.fg);e_drw(&er.bg);
    er.fg_tex = rm_tex(er.fg_tex);
    er.bg_tex = rm_tex(er.bg_tex);
    er.prg = rm_prg(er.prg);
}

static void d_drw(const drawable_t *drw)
{
    glBindTexture(GL_TEXTURE_2D, drw->tex);
    glUniform2f(er.locs[VAR_SCL], drw->scl.x, drw->scl.y);
    glUniform2f(er.locs[VAR_ROT], drw->rot.x, drw->rot.y);
    glUniform2f(er.locs[VAR_MOV], drw->mov.x, drw->mov.y);
    glBindBuffer(GL_ARRAY_BUFFER, drw->scrpts);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, drw->texpts);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drw->indices);
    glDrawElements(GL_TRIANGLES, drw->no_indices, INDEX_MACRO, 0);
}

static void update_engine_renderer(v2f_t pos, float sin_a, float cos_a, float fuel,bool thrust)
{
    er.bar.scl.x = fuel;
    er.hero.mov.x = -1 + (1.0f + pos.x * 2) / e.levels[e.i].bmp.w;
    er.hero.mov.y = -1 + (1.0f + pos.y * 2) / e.levels[e.i].bmp.h ;
    er.hero.rot.x = sin_a;
    er.hero.rot.y = cos_a;
    if (thrust)
    {
        er.flame.scl.x = 1.0f / e.levels[e.i].bmp.w;
        er.flame.scl.y = 1.0f / e.levels[e.i].bmp.h;
        er.flame.mov = er.hero.mov;
        er.flame.rot = er.hero.rot;
    }
    else
    {
        er.flame.scl.x = er.flame.scl.y = 0;
    }
}

void draw_engine_renderer_level(v2f_t pos, float sin_a, float cos_a, float fuel, bool thrust)
{
    update_engine_renderer(pos, sin_a, cos_a, fuel, thrust);
    glClear(GL_COLOR_BUFFER_BIT);
    d_drw(&er.bg);d_drw(&er.fg);d_drw(&er.bar);d_drw(&er.hero);d_drw(&er.flame);
    glfwSwapBuffers(er.win);
}

GLFWwindow *engine_renderer_window(void)
{
    return er.win;
}
