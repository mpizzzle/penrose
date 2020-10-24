//
// © 2020 Michael Percival <m@michaelpercival.xyz>
//   See LICENSE file for copyright and license details.
//

#version 330 core

uniform vec3 paint;

void main() {
    gl_FragColor = vec4(paint, 1.0f);
}
