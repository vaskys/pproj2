cmake_minimum_required(VERSION 3.26)
set(PROJECT_NAME fraktal)

project(${PROJECT_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)

FILE(GLOB_RECURSE source src/*.cpp)

if (CMAKE_C_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Xclang -fopenmp)
endif()

find_package(MPI REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

link_directories(
    /opt/homebrew/opt/libomp/lib
    /opt/homebrew/opt/glew/lib
)

include_directories(
    include
    imgui
    imgui/backends
    implot
    /opt/homebrew/opt/libomp/include
    SYSTEM ${MPI_INCLUDE_PATH}
    SYSTEM ${GLEW_INCLUDE_DIR}
    SYSTEM ${glfw_INCLUDE_PATH}
)

add_executable(fraktal
    ${source}
    imgui/imgui.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/imgui_demo.cpp

    implot/implot.cpp
    implot/implot_items.cpp
)

target_link_libraries(fraktal omp)
target_link_libraries(fraktal ${MPI_C_LIBRARIES})
target_link_libraries(fraktal ${OPENGL_LIBRARIES})
target_link_libraries(fraktal glew)
target_link_libraries(fraktal glfw)
