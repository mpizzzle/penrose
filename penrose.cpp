#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "shader.hpp"

//class triangle {
//public:
//    SDL_Point* points;
//    std::vector<triangle*> subtriangles;
//};
//
//class t123 : public triangle {
//};
//
//class t124 : public triangle {
//};
//
//void draw(SDL_Renderer* Renderer, std::vector<triangle> triangles) {
//    SDL_SetRenderDrawColor(Renderer, 0x33, 0x66, 0x99, SDL_ALPHA_OPAQUE);
//    SDL_RenderClear(Renderer);
//
//    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
//
//    for (triangle t : triangles) {
//        SDL_RenderDrawLines(Renderer, t.points, 4);
//    }
//
//    SDL_RenderPresent(Renderer);
//}

int main() {
    // An array of 3 vectors which represents 3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
       -.5f, -.5f, 0.0f,
        .5f, -.5f, 0.0f,
       0.0f,  .5f, 0.0f,
    };

    //std::vector<triangle> triangles;
    //t123 t;
    //SDL_Point points[4] = { {0, 480}, {320, 0}, {640, 480}, {0, 480} };
    //t.points = points;
    //triangles.push_back(t);

    // Initialise GLFW
    glewExperimental = true;
    if(!glfwInit())
    {
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

    // Open a window and create its OpenGL context
    GLFWwindow* window; // (In the accompanying source code, this variable is global for simplicity)
    window = glfwCreateWindow(1920, 1080, "penrose", NULL, NULL);

    if(window == NULL) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental=true;

    if (glewInit() != GLEW_OK) {
        return -1;
    }

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint programID = Shader::loadShaders("vertex.vert", "fragment.frag");
    glUseProgram(programID);

    do{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
           0,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
           3,        // size
           GL_FLOAT, // type
           GL_FALSE, // normalized?
           0,        // stride
           (void*)0  // array buffer offset
        );
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    return 0;
}
