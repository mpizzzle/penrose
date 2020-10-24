#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <array>
#include <vector>

#include "shader.hpp"

static const uint32_t window_w = 1920;
static const uint32_t window_h = 1080;
static const uint32_t depth = 6;       //recursion depth
static const bool p2 = false;          //tiling type (p2, p3)
static const float line_width = 2.0f;
static const float phi = 1.0 / ((1.0 + sqrt(5.0)) / 2);

static const std::array<glm::vec3, 4> colours = //primary, secondary, line, background
    { glm::vec3(0.7f, 0.0f, 0.35f), glm::vec3(0.35f, 1.0f, 0.7f), glm::vec3(0.0f, 0.35, 0.35), glm::vec3 (0.35f, 0.15f, 0.35f) };

class triangle {
public:
    bool t_123;
    std::array<uint32_t, 3> indices;
    std::vector<triangle*> sub_triangles;

    triangle(bool t_123, std::array<uint32_t, 3> indices) {
        this->t_123 = t_123;
        this->indices = indices;
    }
};

void split(triangle& p, std::vector<glm::vec2>& points, std::array<std::vector<uint32_t>, 3>& indices, uint32_t depth) {
    uint32_t s = points.size();
    std::array<uint32_t, 3>& i = p.indices;

    if (depth > 0) {
        if (p.t_123 ^ !p2) {
            points.push_back(glm::vec2(((1.0f - phi) * points[i[0]]) + (phi * points[i[2]])));
            points.push_back(glm::vec2(((1.0f - phi) * points[i[p2]]) + (phi * points[i[!p2]])));

            triangle t1(p2, std::array<uint32_t, 3>({ i[(!p2) + 1], p2 ? i[2] : s, p2 ? s : i[1] }));
            triangle t2(true, std::array<uint32_t, 3>({ p2 ? i[1] : s, s + 1, p2 ? s : i[1] }));
            triangle t3(false, std::array<uint32_t, 3>({ s, s + 1, i[0] }));

            p.sub_triangles = { &t1, &t2, &t3 };
        }
        else {
            points.push_back(glm::vec2(((1.0f - phi) * points[i[p2 * 2]]) + (phi * points[i[!p2]])));

            triangle t1(true, std::array<uint32_t, 3>({ i[2], s, i[1] }));
            triangle t2(false, std::array<uint32_t, 3>({ i[(!p2) + 1], s, i[0] }));

            p.sub_triangles = { &t1, &t2 };
        }

        for (auto& t : p.sub_triangles) {
            if (depth == 1) {
                for (uint32_t k = 0; k < 3; ++k) {
                    if (k != (t->t_123 ^ !p2 ? 2 : 1)) {
                        indices[2].push_back(t->indices[k]);
                        indices[2].push_back(t->indices[((k + 1) % 3)]);
                    }
                }

                indices[t->t_123].insert(indices[t->t_123].end(), t->indices.begin(), t->indices.end());
            }

            split(*t, points, indices, depth - 1);
        }
    }

    return;
}
 
int main() {
    uint32_t poly = 10;
    float poly_angle = glm::radians(360.0f / poly);

    std::vector<glm::vec2> points = { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f) };
    std::array<std::vector<uint32_t>, 3> indices;

    for (uint32_t i = 1; i < poly; ++i) {
        glm::vec2 next = glm::rotate(points[i], poly_angle);
        points.push_back(next);
    }

    for (auto& p : points) {
        p = glm::rotate(p, poly_angle);
        p.x = (p.x / window_w) * window_h;
    }

    for (uint32_t i = 0; i < poly; i++) {
        std::array<uint32_t, 2> p = { (i % (poly + 1)) + 1, ((i + 1) % poly) + 1 };

        triangle t(true, std::array<uint32_t, 3>({ 0, p[i & 1], p[!(i & 1)] }));

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

    uint32_t VAOs[3], VBO, EBOs[3];

    glGenVertexArrays(3, VAOs);
    glGenBuffers(1, &VBO);
    glGenBuffers(3, EBOs);
    glLineWidth(line_width);

    for (int i = 0; i < 3; ++i) {
        glBindVertexArray(VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, points.size() * 4 * 2, &points[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * 4, &indices[i][0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    uint32_t programID = Shader::loadShaders("vertex.vert", "fragment.frag");
    GLint paint = glGetUniformLocation(programID, "paint");

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 && paint != -1) {
        glClearColor(colours[3].x, colours[3].y, colours[3].z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programID);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        for (int i = 0; i < 3; ++i) {
            glPolygonMode(GL_FRONT_AND_BACK, i < 2 ? GL_FILL : GL_LINE);
            glUniform3fv(paint, 1, &colours[i][0]);
            glBindVertexArray(VAOs[i]);
            glDrawElements(i < 2 ? GL_TRIANGLES : GL_LINES, indices[i].size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
