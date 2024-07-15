#include "ogl.h"
#include "config.h"
#include "gui.h"
#include "framebuffer.h"
#include "mb.h"
#include "implot.h"
#include <mpi.h>
#include <string>
#include <mpi.h>

vector<MB*> mb_objs;
MB* active_obj;
Shader* mb_shader;

GLFWwindow *window;
unsigned int VAO;
unsigned int VBO;

int screen_w = 0;
int screen_h = 0;

int d_screen_w = 0;
int d_screen_h = 0;


double mouse_x = 0;
double mouse_y = 0;

Shader *fbo_shader;

bool key_shift = false;

double prev_time = 0;
double frames=0;
int delta_time = 0;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    float zoom = active_obj->get_zoom();
    float offset_x = active_obj->get_offset_x();
    float offset_y = active_obj->get_offset_y();

    if(key_shift) {
        zoom +=(yoffset * 0.1f) * (zoom/2.0f);
        if(zoom < 100 ) zoom = 100;

    }
    else {
        offset_y -= (yoffset * 1) / zoom;
        offset_x += (xoffset * 1) / zoom;
    }

    active_obj->set_zoom(zoom);
    active_obj->set_offset_x(offset_x);
    active_obj->set_offset_y(offset_y);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    if (key == GLFW_KEY_LEFT_SHIFT  && action == GLFW_PRESS ) {
        key_shift = true;
    }

    if (key == GLFW_KEY_LEFT_SHIFT  && action == GLFW_RELEASE ) {
        key_shift = false;
    }
}


void init_g_object() {
    float quad[] = { 
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void g_draw_g_object() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

int g_init(int screenW, int screenH, const char *title) {
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(screenW, screenH, title, NULL, NULL);
    glfwMakeContextCurrent(window);

    glewInit();
    init_gui(window);
    init_g_object();

    screen_w = screenW;
    screen_h = screenH;

    fbo_shader = new Shader("shaders/fbo_vertex.glsl","shaders/fbo_fragment.glsl");
    mb_shader = new Shader("shaders/mb_vertex.glsl","shaders/mv_fragment.glsl");

    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    g_add_mb_obj();
    g_set_active_mb_obj(mb_objs.front());

    prev_time = glfwGetTime() * 1000;

    glfwSwapInterval(0);

    return 0;
}

void g_clear() {
    delete fbo_shader;
    delete mb_shader;

    for(int i = 0; i < mb_objs.size(); i++ ) {
        mb_objs[i]->clear();
    }

    for (auto p : mb_objs)
    {
        delete p;
    } 
    mb_objs.clear();


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwTerminate();
}

bool g_main_loop() {
    if(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glfwGetFramebufferSize( window, &d_screen_w, &d_screen_h);
        glfwGetCursorPos(window, &mouse_x, &mouse_y);


        delta_time = glfwGetTime() * 1000 - prev_time;
        frames++;

        if (delta_time >= 1000.0) {
            if(g_get_active_mb_obj()->get_type() == 0) {
                g_get_active_mb_obj()->cas = delta_time/double(frames);
                if(g_get_active_mb_obj()->gpu_data.size() < 10 ) {
                    g_get_active_mb_obj()->gpu_data.push_back(g_get_active_mb_obj()->cas);
                }
            }

            frames = 0;
            prev_time = glfwGetTime() * 1000;
        }

        return true;
    }
    return false;
}

void g_swap_buffer() {

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    #if GUI == 1 
        draw_gui();
    #else
        fbo_shader->use();
        glActiveTexture(GL_TEXTURE0+active_buffer->texture);
        fbo_shader->send_int_uniform("fbo_buffer",active_buffer->texture);        
        glBindTexture(GL_TEXTURE_2D, active_buffer->texture);
        g_draw_g_object();
    #endif

    glfwSwapBuffers(window);
}

void g_clear_color(float r, float g, float b) {
    glBindFramebuffer(GL_FRAMEBUFFER, active_obj->get_buffer()->id);
    glClearColor(r,g,b,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    active_obj->update();
}

FrameBuffer* g_get_active_buffer() {
    return active_obj->get_buffer();
}


int g_get_screen_w() {
    return screen_w;
}

int g_get_screen_h() {
    return screen_h;
}

int g_get_d_screen_w() {
    return d_screen_w;
}

int g_get_d_screen_h() {
    return d_screen_h;
}

int g_get_mouse_x() {
    return mouse_x/g_get_screen_w();
}

int g_get_mouse_y() {
    return mouse_y/g_get_screen_h();
}


GLFWwindow *g_get_window() {
    return window;
}


MB* g_get_active_mb_obj() {
    return active_obj;
}

void g_add_mb_obj() {
    MB*obj = new MB();
    mb_objs.push_back(obj);
}

Shader *g_get_mb_shader() {
    return mb_shader;
}


vector<MB*>* g_get_mb_objs() {
    return &mb_objs;
}

void g_set_active_mb_obj(MB *obj) {
    active_obj = obj;
}

int g_get_delta_time() {
    return delta_time;
}

