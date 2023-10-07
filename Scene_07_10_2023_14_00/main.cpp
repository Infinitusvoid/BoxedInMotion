#include "../BoxedInMotion/Engine.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>

const int num_boxes = 1000 * 1000;

float generate_random_float(const float min_value, const float max_value)
{
	return min_value + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value - min_value);
}


glm::vec3 generate_random_glm_vec3(const glm::vec3& min_value, const glm::vec3& max_value)
{
	glm::vec3 result;
	result.x = min_value.x + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value.x - min_value.x);
	result.y = min_value.y + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value.y - min_value.y);
	result.z = min_value.z + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value.z - min_value.z);
	return result;
}

struct Agent
{
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
	static constexpr float box_side = 2.0f;
	static constexpr float half_box_side = box_side * 0.5f;

	void init()
	{
		
		position = generate_random_glm_vec3(glm::vec3(-half_box_side, -half_box_side, -half_box_side), glm::vec3(half_box_side, half_box_side, half_box_side));
		
		float the_velocity_scaling = 0.01f;
		velocity = glm::normalize( generate_random_glm_vec3(glm::vec3(-1.0, -1.0f, -1.0), glm::vec3(1.0f, 1.0f, 1.0f))) * the_velocity_scaling;
	}

	void update(const float dt)
	{
		

		position += velocity;

		float fak_0 = 0.9;
		float fak_1 = 1.0f - fak_0;

		if (position.x < -half_box_side)
		{
			position.x = half_box_side;
			color = glm::vec4(1.0f, color.g, color.b, 1.0) * fak_0 + color * fak_1;
		}

		if (position.y < -half_box_side)
		{
			position.y = half_box_side;
			color = glm::vec4(color.r, 1.0f, color.b, 1.0f) * fak_0 + color * fak_1;
		}

		if (position.z < -half_box_side)
		{
			position.z = half_box_side;
			color = glm::vec4(color.r, color.g, 1.0f, 1.0f) * fak_0 + color * fak_1;
		}


		if (position.x > half_box_side)
		{
			position.x = -half_box_side;
			color = glm::vec4(0.0f, color.g, color.b, 1.0f) * fak_0 + color * fak_1;
		}

		if (position.y > half_box_side)
		{
			position.y = -half_box_side;
			color = glm::vec4(color.r, 0.0f, color.b, 1.0f) * fak_0 + color * fak_1;
		
		}

		if (position.z > half_box_side)
		{
			position.z = -half_box_side;
			color = glm::vec4(color.r, color.g, 0.0f, 1.0f) * fak_0 + color * fak_1;
		}
		
		velocity += generate_random_glm_vec3(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f)) * 0.1f;

		if (length(velocity) > 1.0)
		{
			velocity *= 0.9;
		}
		
	}
};

std::vector<Agent> agents;

void init(Engine::Instance_data* data)
{
	for (int i = 0; i < num_boxes; i++)
	{
		agents.emplace_back(Agent());
		agents[i].init();
	}
	

	for (unsigned int i = 0; i < num_boxes; i++)
	{
		data[i].model = glm::mat4(1.0f);
		auto& model = data[i].model;
		model = glm::translate(model, glm::vec3(i, 0, 0));
		model = glm::scale(model, glm::vec3(0.001f));



		auto& color = data[i].color;
		color = glm::vec4(0.2f, 1.0f, 1.0f, 1.0f);
	}

}

int index = 0;
void loop(Engine::Instance_data* data)
{
	float dt = Engine::get_dt();
	for (int i = 0; i < 20000; i++)
	{
		index++;
		index = index % num_boxes;

		auto& model = data[index].model;
		model = glm::mat4(1.0f);
		
		agents[index].update(dt);
		

		model = glm::translate(model, agents[index].position);

		model = glm::scale(model, glm::vec3(0.001f));

		data[index].color = agents[index].color;
		
	}
}


int main()
{
	Engine::set_background_color(glm::vec3(0.0004f, 0.002f, 0.0017f));
	Engine::set_bloom_iteration(20);
	//Engine::set_bloom_iteration(10);

	Engine::ShaderSourceCode source;


	source.vertex = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"\n"
		"// Define the instance data struct\n"
		"struct Instance_data\n"
		"{\n"
		"mat4 model;\n"
		"vec4 color;\n"
		"};\n"
		"\n"
		"layout(location = 3) in Instance_data instanceData;\n"
		"\n"
		"out vec4 Color;\n"
		"out vec3 FragWorldPos; // World space position\n"
		"out vec3 FragObjectPos; // Object space position\n"
		"\n"
		"uniform mat4 projection;\n"
		"uniform mat4 view;\n"
		"\n"
		"uniform vec3 camera_position;\n"
		"uniform vec3 camera_front;\n"
		"uniform vec3 camera_right;\n"
		"uniform vec3 camera_up;\n"
		"uniform float camera_zoom;\n"
		"uniform int frame;\n"
		"uniform float time;\n"
		"\n"
		"void main()\n"
		"{\n"
		"int id = gl_InstanceID;\n"
		"FragObjectPos = aPos; // Store the object space position\n"
		"FragWorldPos = vec3(instanceData.model * vec4(aPos, 1.0)); // Calculate world space position\n"
		"Color = instanceData.color;\n"
		"gl_Position = projection * view * instanceData.model * vec4(aPos + 0.25f *( sin(time + id * 0.027) + sin(time + id * 0.017)), 1.0f);\n"
		"}\n";

	source.fragment =
		"#version 330 core\n"
		"layout (location = 0) out vec4 FragColor;\n"
		"layout (location = 1) out vec4 BrightColor;\n"
		"\n"
		"in vec4 Color;\n"
		"in vec3 FragWorldPos; // Input world space position\n"
		"in vec3 FragObjectPos; // Input object space position\n"
		"\n"
		"\n"
		"uniform vec3 camera_position;\n"
		"uniform vec3 camera_front;\n"
		"uniform vec3 camera_right;\n"
		"uniform vec3 camera_up;\n"
		"uniform float camera_zoom;\n"
		"uniform int frame;\n"
		"uniform float time;\n"
		"\n"
		"void main()\n"
		"{\n"
		"   vec4 result = Color + vec4(0.0, sin(FragWorldPos.x * 10.0f + time) * cos(FragWorldPos.z * 2.0f + time), sin(FragWorldPos.y * 7.0f + time) * cos(FragWorldPos.z * 4.0f + time), 1.0);\n" 
		"	FragColor = result;"
		"	float factor_brightnes = dot(vec3(result), vec3(0.2126, 0.7152, 0.0722));\n"
		"	if(factor_brightnes > 1.0) // transhold usually set at 1.0\n"
		"	{\n"
		"		BrightColor = vec4(result.x * 10.0, result.y * 10.0, result.z * 10.0, 1.0);\n";
		"	}\n"
		"	else\n"
		"	{\n"
	    "		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
	    "	}\n"
		"}\n";

	
		

	//Engine::set_camera_parameters(glm::vec3(0.0, 0.0, 0.0), 10.0f, 0.2f, 45.0f);
	Engine::play(num_boxes, init, loop, 45.0f, 1000.0f, &source);
	//Engine::play(num_boxes, init, loop, 45.0f, 1000.0f);

	std::cout << "Scene_07_10_2023_14_00\n";
	return 0;
}