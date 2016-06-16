#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "bool.h"
#include "engine_data.h"

void   init_engine_renderer(void);
void   exit_engine_renderer(void);
void   init_engine_renderer_level(void);
void   exit_engine_renderer_level(void);
void   draw_engine_renderer_level(v2f_t pos, float sin_a, float cos_a, float fuel, bool thrust);
GLFWwindow *engine_renderer_window(void);

#endif
