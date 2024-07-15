#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define DEBUG_PRINT 1
#define GUI 1

void print(std::string msg);
void export_image(string file_path);

void clear_all_mb();

