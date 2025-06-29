cmake_minimum_required(VERSION 3.10)
project(YourProject)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(thirdparty)
include_directories(thirdparty/imgui)
include_directories(thirdparty/glad)

add_executable(YourProject
    source/main.cpp
    source/utils/window.cpp
    source/utils/renderer.cpp
        source/utils/spotlight.cpp
            source/utils/mesh.cpp
    source/utils/gamecamera.cpp
    source/utils/shape.cpp
    source/utils/camera.cpp
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/imgui_widgets.cpp
    thirdparty/imgui/imgui_tables.cpp
    thirdparty/imgui/backends/imgui_impl_sdl2.cpp
    thirdparty/imgui/backends/imgui_impl_opengl3.cpp
    thirdparty/stb/stb_image.c 
    thirdparty/glad/glad.c 
    thirdparty/tinyobjloader/tiny_obj_loader.cc
)

target_link_libraries(YourProject ${SDL2_LIBRARIES} OpenGL::GL)