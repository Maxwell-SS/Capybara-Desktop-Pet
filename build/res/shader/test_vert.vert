#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 u_model;

out vec2 TexCoord;

void main() {
   gl_Position = u_model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   // gl_Position = vec4(aPos, 1.0);
   TexCoord = aTexCoord;
}