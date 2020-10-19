#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <array>
#include <iostream>
#include <vector>

#include "shader.hpp"

static const uint32_t window_w = 1920;
static const uint32_t window_h = 1080;

class triangle {
public:
    bool clockwise; //do we need this?
    std::array<glm::vec2*, 3> points;
    std::vector<triangle*> subtriangles;
};

class t123 : public triangle {
};

class t124 : public triangle {
};

int main() {
    uint32_t poly = 11;
    GLfloat poly_angle = glm::radians(360.0f / poly);
    glm::vec2 origin = glm::vec2(0.0f, 0.0f);
    glm::vec2 point = glm::vec2(0.0f, 1.0f);

    std::vector<glm::vec2> points = { origin, point };
    std::vector<uint32_t> indices = { 0, 1, 2 };

    for (uint32_t i = 2; i < poly + 1; ++i) {
        glm::vec2 next = glm::rotate(points[i - 1], poly_angle);
        points.push_back(next);
        std::array<uint32_t, 3> t = { 0, ((i - 1) % (poly + 1)) + 1, (i % poly) + 1 };
        indices.insert(indices.end(), t.begin(), t.end());
    }

    for (auto& p : points) {
        p = glm::rotate(p, poly_angle / 2.0f);
        p.x = (p.x / window_w) * window_h;
    }

    std::vector<triangle> triangles;

    //for (int i = 0; i < 10; ++i) {
    //    t123 t;
    //    t.clockwise = i % 2 == 0;
    //    if (t.clockwise) {
    //        t.points = { glm::rotateZ(point, glm::radians(36.0f)), point, glm::rotateZ(point, glm::radians(-36.0f)) };
    //        point = glm::rotateZ(glm::rotateZ(point, glm::radians(36.0f)), glm::radians(36.0f));
    //    }
    //    else {
    //        t.points = { glm::rotateZ(point, glm::radians(-36.0f)), point, glm::rotateZ(point, glm::radians(36.0f)) };
    //    }
    //    triangles.push_back(t);
    //}

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
    window = glfwCreateWindow(window_w, window_h, "penrose", NULL, NULL);

    if(window == NULL) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental=true;

    if (glewInit() != GLEW_OK) {
        return -1;
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, points.size() * 4 * 2, &points[0], GL_STATIC_DRAW);

    uint32_t EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, &indices[0], GL_STATIC_DRAW);

    GLuint programID = Shader::loadShaders("vertex.vert", "fragment.frag");

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
        glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programID);

        glEnableVertexAttribArray(0);

        glBindVertexArray(vertexArrayID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
