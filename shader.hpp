/*
 * © 2020 Michael Percival <m@michaelpercival.xyz>
 *   See LICENSE file for copyright and license details.
 */

#include <GL/glew.h>

#include <string>

class Shader {
public:
    static GLuint loadShaders(std::string vert_shader, std::string frag_shader);
};
