#version 330 core

// layout(location = 0) in vec3 position;

// flat out vec3 vcolor; // last index convention

void main() {
    gl_Position = projection * view * vec4(position, 1.0f);
}