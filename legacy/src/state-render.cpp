#include "state-render.h"

StateRender::StateRender(/* args */)
{
}

StateRender::~StateRender()
{
}

bool StateRender::initStateRender(SDL_Window *window, void * gl_context)
{
    // Return false is arguments are invalid
    if (window == NULL || gl_context == NULL)
    {
        return false;
    }

    sdl_window = window;
    sdl_gl_context = gl_context;

    // Create the viewport

    return true;
}

bool StateRender::initShaders()
{
    // Vertex Shader source code
    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\0";

    // // Fragment Shader source code
    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
                                       "}\n\0";

    // // Create vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // // Attach Vertex Shader source to the Vertex Shader Object
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    // // Compile the Vertex Shader into machine code
    glCompileShader(vertexShader);

    // // Validate Shader

    // // Add shader to shader list for reference later
    vertexShaderList.push_back(vertexShader);

    // // Create fragment shader
    GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // // Attach Fragment shader source
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    // // Compile the Fragment Shader
    glCompileShader(fragmentShader);

    // // Validate Shader

    // // Create shader program
    GLuint shaderProgram = glCreateProgram();

    // // Attach vertex and fragment shader
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // // Link the shader
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    compiledShaderProgram = shaderProgram;

    return true;
}

bool StateRender::initVertices()
{
    // Create vertex's
    GLfloat initial_vertexes[] = {
        -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,  // Lower left corner
        0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,   // Lower right corner
        0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f // Upper corner
    };


    // // Create reference containers for the Vertex Array Object and the Vertex Buffer Object
    GLuint VAO, VBO;

    // // Generate the VAO and VBO with only 1 object each
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // // Make the VAO the current Vertex Array Object by binding it
    glBindVertexArray(VAO);

    // // Introduce the vertices into the VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(initial_vertexes), initial_vertexes, GL_STATIC_DRAW);

    // // Configure the Vertex Attribute so that OpenGL knows how to read the VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    // // Enable the Vertex Attribute so that OpenGL knows to use it
    glEnableVertexAttribArray(0);

    // // Bind both the VBO and VAO to 0 so that we don't accidentally modify the VAO and VBO we created
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Vbo = VBO;
    Vao = VAO;

    return true;
}

bool StateRender::mainLoop()
{
    // Specify the color of the background
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    // Clean the back buffer and assign the new color to it
    glClear(GL_COLOR_BUFFER_BIT);
    // Tell OpenGL which Shader Program we want to use
    glUseProgram(compiledShaderProgram);
    // Bind the VAO so OpenGL knows to use it
    glBindVertexArray(Vao);
    // Draw the triangle using the GL_TRIANGLES primitive
    glDrawArrays(GL_TRIANGLES, 0, 3);

    return true;
}