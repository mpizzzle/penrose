#version 330 core

uniform vec3 paint;

void main() {
    gl_FragColor = vec4(paint, 1.0f);
}
