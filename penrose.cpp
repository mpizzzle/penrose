#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <array>
#include <vector>

#include "shader.hpp"

static const uint32_t window_w = 1920;
static const uint32_t window_h = 1080;
static const uint32_t depth = 6;

static const glm::vec4 primary(0.7f, 0.7f, 0.0f, 1.0f);
static const glm::vec4 secondary(0.2f, 0.2f, 0.4f, 1.0f);
static const glm::vec4 black(0.0f, 0.5f, 0.0f, 1.0f);
static const glm::vec4 background(0.15f, 0.15f, 0.35f, 1.0f);

static const GLfloat phi = 1.0 / ((1.0 + sqrt(5.0)) / 2);

struct triangle {
    bool t_123;
    std::array<uint32_t, 3> indices;
    std::array<glm::vec2, 3> points;
    std::vector<triangle*> subtriangles;
};

void split(triangle& parent, std::vector<glm::vec2>& points, std::vector<uint32_t>& t123_indices, std::vector<uint32_t>& t124_indices, std::vector<uint32_t>& line_indices, uint32_t depth) {
    uint32_t s = points.size();
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

            t = { i[1], i[2], s,    //t123 1
                  i[1], s + 1, s,   //t123 2
                  s, s + 1, i[0] }; //t124

            triangle t123_1;
            t123_1.t_123 = true;
            t123_1.points = { parent.points[1], parent.points[2], p1 };
            t123_1.indices = { i[1], i[2], s };

            triangle t123_2;
            t123_2.t_123 = true;
            t123_2.points = { parent.points[1], p2, p1 };
            t123_2.indices = { i[1], s + 1, s };

            triangle t124;
            t124.t_123 = false;
            t124.points = { p1, p2, parent.points[0] };
            t124.indices = { s, s + 1, i[0] };

            parent.subtriangles = { &t123_1, &t123_2, &t124 };
        }
        else {
            glm::vec2 p3(((1.0f - phi) * p[2].x) + (phi * p[0].x), ((1.0f - phi) * p[2].y) + (phi * p[0].y));

            points.push_back(p3);

            t = { i[2], s, i[1],   //t123
                  i[1], s, i[0] }; //t124

            triangle t123;
            t123.t_123 = true;
            t123.points = { parent.points[2], p3, parent.points[1] };
            t123.indices = { i[2], s, i[1] };

            triangle t124;
            t124.t_123 = false;
            t124.points = { parent.points[1], p3, parent.points[0] };
            t124.indices = { i[1], s, i[0] };

            parent.subtriangles = { &t123, &t124 };
        }

        if (depth == 1) {
            for (auto& tri : parent.subtriangles) {
                for (uint32_t k = 0; k < 3; ++k) {
                    if (k != (tri->t_123 ? 2 : 1)) {
                        line_indices.push_back(tri->indices[k]);
                        line_indices.push_back(tri->indices[((k + 1) % 3)]);
                    }
                }

                if (tri->t_123) {
                    t123_indices.insert(t123_indices.end(), tri->indices.begin(), tri->indices.end());
                }
                else {
                    t124_indices.insert(t124_indices.end(), tri->indices.begin(), tri->indices.end());
                }
            }
        }

        for (auto& tri : parent.subtriangles) {
            split(*tri, points, t123_indices, t124_indices, line_indices, depth - 1);
        }
    }

    return;
}
 
int main() {
    uint32_t poly = 10;
    GLfloat poly_angle = glm::radians(360.0f / poly);
    glm::vec2 origin = glm::vec2(0.0f, 0.0f);
    glm::vec2 point = glm::vec2(0.0f, 1.0f);

    std::vector<triangle> triangles;
    std::vector<glm::vec2> points = { origin, point };
    std::vector<uint32_t> t123_indices;
    std::vector<uint32_t> t124_indices;
    std::vector<uint32_t> line_indices;

    for (uint32_t i = 1; i < poly; ++i) {
        glm::vec2 next = glm::rotate(points[i], poly_angle);
        points.push_back(next);
    }

    for (auto& p : points) {
        p = glm::rotate(p, poly_angle);
        p.x = (p.x / window_w) * window_h;
    }

    for (uint32_t i = 0; i < poly; i++) {
        std::array<uint32_t, 2> indices = { (i % (poly + 1)) + 1, ((i + 1) % poly) + 1 };
        if (i & 1) std::swap(indices[0], indices[1]);

        triangle t;
        t.t_123 = true;
        t.points = { points[0], points[indices[0]], points[indices[1]] };
        t.indices = { 0, indices[0], indices[1] };

        split(t, points, t123_indices, t124_indices, line_indices, depth);
    }

    if(!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(window_w, window_h, "penrose", NULL, NULL);

    if(window == NULL) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental=true;

    if (glewInit() != GLEW_OK) {
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    GLuint VAOs[3], VBO, EBOs[3];

    glGenVertexArrays(3, VAOs);
    glGenBuffers(1, &VBO);
    glGenBuffers(3, EBOs);

    glBindVertexArray(VAOs[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * 4 * 2, &points[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, t123_indices.size() * 4, &t123_indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAOs[1]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, t124_indices.size() * 4, &t124_indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAOs[2]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, line_indices.size() * 4, &line_indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint programID = Shader::loadShaders("vertex.vert", "fragment.frag");
    GLint paint = glGetUniformLocation(programID, "paint");

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 && paint != -1) {
        glClearColor(background.x, background.y, background.z, background.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programID);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glUniform4fv(paint, 1, &secondary[0]);
        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, t123_indices.size(), GL_UNSIGNED_INT, 0);

        glUniform4fv(paint, 1, &primary[0]);
        glBindVertexArray(VAOs[1]);
        glDrawElements(GL_TRIANGLES, t124_indices.size(), GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glUniform4fv(paint, 1, &black[0]);
        glBindVertexArray(VAOs[2]);
        glDrawElements(GL_LINES, line_indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
