#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
in vec4 Color;
in vec3 FragWorldPos; // Input world space position
in vec3 FragObjectPos; // Input object space position


uniform vec3 camera_position;
uniform vec3 camera_front;
uniform vec3 camera_right;
uniform vec3 camera_up;
uniform float camera_zoom;
uniform int frame;
uniform float time;

void main()
{
   vec4 result = Color + sin(time + FragWorldPos.x * 10.0) * 0.7 + vec4(vec3(sin(FragObjectPos.x * 100), 0.0, 0.0), 1.0);
	FragColor = result * 0.7;
   float factor_brightnes = dot(vec3(result), vec3(0.2126, 0.7152, 0.0722));
   if(factor_brightnes > 1.0) // transhold usually set at 1.0
   {
     BrightColor = vec4(result.x * 10.0, result.y * 10.0, result.z * 10.0, 1.0);
   }
   else
   {
     BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
   }

}
