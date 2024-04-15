#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform vec3 lightPos;

void main() {
    gl_Position = model * vec4(aPos, 1.0) - vec4(lightPos, 0.0);
}
