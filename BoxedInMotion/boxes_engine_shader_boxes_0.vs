#version 330 core
layout (location = 0) in vec3 aPos;

// Define the instance data struct
struct Instance_data
{
    mat4 model;
    vec4 color;
};

layout (location = 3) in Instance_data instanceData;

out vec4 Color;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    Color = instanceData.color;
    gl_Position = projection * view * instanceData.model * vec4(aPos, 1.0f); 
}