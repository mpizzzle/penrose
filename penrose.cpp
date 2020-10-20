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
static const uint32_t depth = 5;

static const GLfloat phi = 1.0 / ((1.0 + sqrt(5.0)) / 2);

class triangle {
public:
    bool t_123;
    std::array<uint32_t, 3> indices;
    std::array<glm::vec2, 3> points;
    std::vector<triangle*> subtriangles;
};

void split(triangle& parent, std::vector<glm::vec2>& points, std::vector<uint32_t>& indices, uint32_t depth) {
    //uint32_t s = points.size();
    std::array<glm::vec2, 3>& p = parent.points;
    std::array<uint32_t, 3>& i = parent.indices;
    std::vector<uint32_t> t;

    if (depth > 0) {
        if (parent.t_123) {
            //distance d = √((x1 − x0)2 + (y1 − y0)2)
            //ratio t = dt / d, in this case dt = phi * d so t = phi (no need to calc d)
            //therefore, (xt, yt) = (((1 − phi) x0 + phi * x1),((1 − phi) y0 + phi * y1))

            glm::vec2 p1(((1.0f - phi) * p[0].x) + (phi * p[2].x), ((1.0f - phi) * p[0].y) + (phi * p[2].y));
            glm::vec2 p2(((1.0f - phi) * p[1].x) + (phi * p[0].x), ((1.0f - phi) * p[1].y) + (phi * p[0].y));

            points.push_back(p1);
            points.push_back(p2);

            uint32_t s = points.size();

            //t = { i[1], i[2], s,    //t123 1
            //      i[1], s, s + 1,   //t123 2
            //      i[0], s, s + 1 }; //t124

            t = { i[1], i[2], s - 2,    //t123 1
                  i[1], s - 1, s - 2,   //t123 2
                  i[0], s - 2, s - 1 }; //t124

            triangle t123_1;
            t123_1.t_123 = true;
            t123_1.points = { parent.points[1], parent.points[2], points[s - 2] };
            t123_1.indices = { i[1], i[2], s - 2 };

            triangle t123_2;
            t123_2.t_123 = true;
            t123_2.points = { parent.points[1], points[s - 1], points[s - 2] };
            t123_2.indices = { i[1], s - 1, s - 2 };

            triangle t124;
            t124.t_123 = false;
            t124.points = { parent.points[0], points[s - 2], points[s - 1] };
            t124.indices = { i[0], s - 2, s - 1 };

            parent.subtriangles = { &t123_1, &t123_2, &t124 };
        }
        //else {
        //    glm::vec2 p3(((1.0f - phi) * p[1].x) + (phi * p[0].x), ((1.0f - phi) * p[1].y) + (phi * p[0].y));

        //    uint32_t s = points.size();
        //    points.push_back(p3);

        //    t = { i[0], s, s + 1,   //t123
        //          i[1], s, s + 1 }; // t124

        //    triangle t123;
        //    t123.t_123 = true;
        //    t123.points = { parent.points[0], points[s], points[s + 1] };
        //    t123.indices = { 0, s, s + 1 };

        //    triangle t124;
        //    t124.t_123 = false;
        //    t124.points = { parent.points[1], points[s], points[s + 1] };
        //    t124.indices = { 1, s, s + 1 };

        //    parent.subtriangles = { &t123, &t124 };
        //}

        indices.insert(indices.end(), t.begin(), t.end());

        std::cout << "all:" << std::endl;
        for (auto& blep : points) {
                std::cout << "(" << blep.x << ", " << blep.y << ")" << std::endl;
        }
        std::cout << std::endl;

        for (auto&tri : parent.subtriangles) {
            split(*tri, points, indices, depth - 1);
        }
    }

    return;
}

int main() {
    uint32_t poly = 10;
    GLfloat poly_angle = glm::radians(360.0f / poly);
    glm::vec2 origin = glm::vec2(0.0f, 0.0f);
    glm::vec2 point = glm::vec2(0.0f, 1.0f);

    std::vector<glm::vec2> points = { origin, point };
    std::vector<uint32_t> indices = { 0, 1, 2 };

    for (uint32_t i = 2; i < poly + 1; ++i) {
        glm::vec2 next = glm::rotate(points[i - 1], poly_angle);
        points.push_back(next);
        std::array<uint32_t, 3> t = { 0, ((i - 1) % (poly + 1)) + 1, (i % poly) + 1 };
        if (i % 2 == 0) std::swap(t[1], t[2]);
        indices.insert(indices.end(), t.begin(), t.end());
    }

    for (auto& p : points) {
        //p = glm::rotate(p, poly_angle / 2.0f);
        p.x = (p.x / window_w) * window_h;
    }

    std::vector<triangle> triangles;

    for (uint32_t j = 0; j < poly; j++) {
        triangle t;
        t.t_123 = true;
        t.points = { points[indices[3 * j]], points[indices[(3 * j) + 1]], points[indices[(3 * j) + 2]] };
        t.indices = { indices[3 * j], indices[(3 * j) + 1], indices[(3 * j) + 2] };
        split(t, points, indices, depth);
    }

    if(!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow(window_w, window_h, "penrose", NULL, NULL);

    if(window == NULL) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
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
