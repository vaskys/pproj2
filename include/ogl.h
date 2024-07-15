#ifndef OGL_H
#define OGL_H

#include "framebuffer.h"
#include "config.h"
#include "mb.h"
#include "shader.h"

int g_init(int screenW,int screenH,const char* title);
void g_clear();

bool g_main_loop();
void g_clear_color(float r,float g,float b);
void g_swap_buffer();
void g_draw_g_object();
FrameBuffer* g_get_active_buffer();

int g_get_screen_w();
int g_get_screen_h();

int g_get_d_screen_w();
int g_get_d_screen_h();

int g_get_mouse_x();
int g_get_mouse_y();


GLFWwindow *g_get_window();

void g_add_mb_obj();
MB* g_get_active_mb_obj();
void g_set_active_mb_obj(MB* obj);
Shader* g_get_mb_shader();
vector<MB*>* g_get_mb_objs();

int g_get_delta_time();

#endif
