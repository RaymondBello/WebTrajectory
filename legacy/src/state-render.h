#pragma once

#define GL_GLEXT_PROTOTYPES

#include "common.h"
#include "SDL_opengl.h"
#include <imgui/imgui.h>
#include <imgui-extra/imgui_impl.h>

#define MAX_VERTEXES 100

class StateRender
{
private:
    /* data */
public:
    SDL_Window *sdl_window = nullptr;
    void *sdl_gl_context = nullptr;

    // Vertices
    std::vector<GLuint> vertexShaderList;
    std::vector<GLuint> fragmentShaderList;

    // Compiled Shader program
    GLuint compiledShaderProgram;

    // Vertex Array Object
    GLuint Vao;

    // Vertex Buffer Object
    GLuint Vbo;

    GLfloat vertexes[MAX_VERTEXES];

    StateRender(/* args */);
    ~StateRender();

    bool initStateRender(SDL_Window *window, void *gl_context);
    bool initVertices();
    bool initShaders();

    bool mainLoop();
};
