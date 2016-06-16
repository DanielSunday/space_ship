#ifndef GUTIL_H
#define GUTIL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "bool.h"

#define GUTIL_RESIZABLE             (GL_FALSE)
#define GUTIL_CONTEXT_VERSION_MAJOR (3)
#define GUTIL_CONTEXT_VERSION_MINOR (3)
#define GUTIL_OPENGL_FORWARD_COMPAT (GL_TRUE)
#define GUTIL_OPENGL_PROFILE        (GLFW_OPENGL_CORE_PROFILE)
#define GUTIL_GLEW_EXPERIMENTAL     (GL_TRUE)

GLFWwindow *mk_win(int w, int h, const char *title, bool fscr);
GLFWwindow *rm_win(GLFWwindow *win);
GLuint mk_vao(int no_arrs);
GLuint rm_vao(GLuint vao, int no_arrs);
GLuint mk_prg(const char*v,const char*f,int vars,const char**names,GLint*locs);
GLuint rm_prg(GLuint prg);
GLuint mk_vbo(GLenum type, size_t sz, const void *data, GLenum usage);
GLuint rm_vbo(GLuint vbo);
GLuint mk_tex(const char *png_addr);
GLuint rm_tex(GLuint tex);

#endif
