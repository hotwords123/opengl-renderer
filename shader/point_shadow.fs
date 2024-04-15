#version 330 core
in vec4 FragPos;

uniform float far;

void main() {
    float lightDistance = length(FragPos.xyz);
    gl_FragDepth = lightDistance / far;
}
