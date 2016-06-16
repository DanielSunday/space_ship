#include "gutil.h"
#include "upng.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

GLFWwindow *mk_win(int w, int h, const char *title, bool fscr)
{
    GLFWwindow *win;
    GLFWmonitor *mon;
    glfwWindowHint(GLFW_RESIZABLE, GUTIL_RESIZABLE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GUTIL_CONTEXT_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GUTIL_CONTEXT_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GUTIL_OPENGL_FORWARD_COMPAT);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GUTIL_OPENGL_PROFILE);
    glewExperimental = GUTIL_GLEW_EXPERIMENTAL;
    mon = fscr ? glfwGetPrimaryMonitor() : NULL;
    win = glfwCreateWindow(w, h, title, mon, NULL);
    glfwMakeContextCurrent(win);
    glewInit();
    return win;
}

GLFWwindow *rm_win(GLFWwindow *win)
{
    glfwDestroyWindow(win);
    return NULL;
}

GLuint mk_vao(int no_arrs)
{
    GLuint i, vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    for (i = 0; (int) i < no_arrs; ++i) glEnableVertexAttribArray(i);
    return vao;
}

GLuint rm_vao(GLuint vao, int no_arrs)
{
    glDeleteVertexArrays(1, &vao);
    for (vao = 0; (int) vao < no_arrs; ++vao) glDisableVertexAttribArray(vao);
    return 0;
}

static char *get_src(const char *addr)
{
    char *src;
    int len;
    FILE *f;
    f = fopen(addr, "r");
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    rewind(f);
    src = malloc(len + 1);
    src[len] = '\0';
    (void) fread(src, 1, len, f);
    fclose(f);
    return src;
}

GLuint mk_prg(const char*v,const char*f,int vars,const char**names,GLint*locs)
{
    GLuint prg, shdv, shdf, i = 0;
    char *srcv, *srcf;
    prg = glCreateProgram();
    shdv = glCreateShader(GL_VERTEX_SHADER);
    shdf = glCreateShader(GL_FRAGMENT_SHADER);
    srcv = get_src(v);
    srcf = get_src(f);
    glShaderSource(shdv, 1, (const GLchar **) &srcv, NULL);
    glShaderSource(shdf, 1, (const GLchar **) &srcf, NULL);
    glCompileShader(shdv);
    glCompileShader(shdf);
    glAttachShader(prg, shdv);
    glAttachShader(prg, shdf);
    glLinkProgram(prg);
    glDetachShader(prg, shdf);
    glDetachShader(prg, shdv);
    glValidateProgram(prg);
    glUseProgram(prg);
    for (; (int) i < vars; ++i) locs[i] = glGetUniformLocation(prg, names[i]);
    free(srcf);
    free(srcv);
    glDeleteShader(shdf);
    glDeleteShader(shdv);
    return prg;
}

GLuint rm_prg(GLuint prg)
{
    glDeleteProgram(prg);
    return 0;
}

GLuint mk_vbo(GLenum type, size_t sz, const void *data, GLenum usage)
{
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(type, vbo);
    glBufferData(type, sz, data, usage);
    return vbo;
}

GLuint rm_vbo(GLuint vbo)
{
    glDeleteBuffers(1, &vbo);
    return 0;
}

static void transfer_upng_to_texture_2d(upng_t *upng)
{
    GLenum f;
    unsigned w, h, y, sz, row;
    const unsigned char *original;
          unsigned char *inverted;
    f = (upng_get_format(upng) == UPNG_RGB8) ? GL_RGB : GL_RGBA;
    w = upng_get_width(upng);
    h = upng_get_height(upng);
    sz = upng_get_size(upng);
    row = sz / h;
    original = upng_get_buffer(upng);
    inverted = malloc(sz);
    for (y=0;y<h;++y)(void)memcpy(&inverted[y*row],&original[(h-y-1)*row],row);
    glTexImage2D(GL_TEXTURE_2D, 0, f, w, h, 0, f, GL_UNSIGNED_BYTE, inverted);
    free(inverted);
}

GLuint mk_tex(const char *png_addr)
{
    GLuint tex;
    upng_t *upng;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    upng = upng_new_from_file(png_addr);
    (void) upng_decode(upng);
    transfer_upng_to_texture_2d(upng);
    upng_free(upng);
    return tex;
}

GLuint rm_tex(GLuint tex)
{
    glDeleteTextures(1, &tex);
    return 0;
}
