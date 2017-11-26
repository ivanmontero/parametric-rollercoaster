#version 330 core

out vec4 color;

flat in vec3 vcolor; // last index convention

void main() {
    color = vec4(vcolor, 1);
}