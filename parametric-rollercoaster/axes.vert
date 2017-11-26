#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;

flat out vec3 vcolor; // last index convention

void main() {
    gl_Position = projection * view * vec4(position, 1.0f);
    if(position.x != 0)         vcolor = vec3(0, 0, 1);
    else if(position.y != 0)    vcolor = vec3(0, 1, 0);
    else if(position.z != 0)    vcolor = vec3(1, 0, 0);
    else                        vcolor = vec3(0, 0, 0);
} 