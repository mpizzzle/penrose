#include <GL/glew.h>

#include <string>

class Shader {
public:
    static GLuint loadShaders(std::string vert_shader, std::string frag_shader);
};
