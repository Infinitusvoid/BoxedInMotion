#version 330 core
layout (location = 0) in vec3 aPos;

// Define the instance data struct
struct Instance_data
{
	mat4 model;
	vec4 color;
};

layout(location = 3) in Instance_data instanceData;

out vec4 Color;
out vec3 FragWorldPos; // World space position
out vec3 FragObjectPos; // Object space position

uniform mat4 projection;
uniform mat4 view;

uniform vec3 camera_position;
uniform vec3 camera_front;
uniform vec3 camera_right;
uniform vec3 camera_up;
uniform float camera_zoom;
uniform int frame;
uniform float time;

void main()
{
	int id = gl_InstanceID;
	FragObjectPos = aPos; // Store the object space position
	FragWorldPos = vec3(instanceData.model * vec4(aPos, 1.0)); // Calculate world space position
	Color = instanceData.color;
	gl_Position = projection * view * instanceData.model * vec4(aPos + sin(time) * 10.0, 1.0f);
}
