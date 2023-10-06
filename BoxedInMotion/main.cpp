
#include "Boxes_engine.h"

#include <iostream>
#include <string>


const int amount = 100000;

//const int index_a = 0;
//const int size_a = 2000;
//const int index_b = 2000;
//const int size_b = 2000;

const int index_a = 0;
const int size_a = amount / 100;
const int index_b = size_a;
const int size_b = amount - index_b;


bool first = true;

glm::vec3 generate_random_glm_vec3(const glm::vec3& min_value, const glm::vec3& max_value)
{
	glm::vec3 result;
	result.x = min_value.x + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value.x - min_value.x);
	result.y = min_value.y + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value.y - min_value.y);
	result.z = min_value.z + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value.z - min_value.z);
	return result;
}

float generate_random_float(const float min_value, const float max_value)
{
	return min_value + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value - min_value);
}

void f_loop(Boxes_engine::Instance_data* data)
{
	float radius = 150.0;
	float offset = 25.0f;

	float the_offset = Boxes_engine::get_total_time();


	{
		if (first)
		{
			first = false;

			for (unsigned int i = index_a; i < (index_a + size_a); i++)
			{
				data[i].color = glm::vec4(generate_random_glm_vec3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0)) * 10.0f * glm::vec3(0.002f, 0.7f, 0.5f), 1.0f);
			}
		}
		else
		{
			float factor = 1.0f / size_a;
			float total_t = Boxes_engine::get_total_time();

			for (unsigned int i = index_a; i < (index_a + size_a); i++)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(cos(i * factor * 3.1415f * 2.0f + total_t * 0.04210f + i * 2.1717) * 25, sin(i * factor * 3.1415f * 2.0f + total_t * 0.0025f + i * 1.2) * 25, 25 * sin(i * factor * 3.1415f * 2.0f + total_t * 0.012 + i * 1.7)));
				model = glm::scale(model, glm::vec3(0.25f));
				//shaderLight.setMat4("model", model);
				data[i].model = model;
				//glm::vec3 color = generate_random_glm_vec3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)) * 10.0f * glm::vec3(0.2f, 1.0f, 1.0f);
				//glm::vec3 color = colors_cubes[i];
				//shaderLight.setVec3("lightColor", color);
				//renderCube();
			}
		}

	}



	{
		if (Boxes_engine::get_frame() < 10)
		{
			for (unsigned int i = index_b; i < (index_b + size_b); i++)
			{
				glm::mat4 model = glm::mat4(1.0f);
				// 1. translation: displace along circle with 'radius' in range [-offset, offset]
				float angle = (float)i / (float)size_b * 360.0f;
				float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset + sinf(the_offset) * 10.0f;
				float x = sin(angle + the_offset) * radius + displacement;
				displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
				float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
				displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
				float z = cos(angle + the_offset) * radius + displacement;
				model = glm::translate(model, glm::vec3(x, y, z));

				// 2. scale: Scale between 0.05 and 0.25f
				float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
				model = glm::scale(model, glm::vec3(scale) * 1.0f);

				// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
				float rotAngle = static_cast<float>((rand() % 360));
				model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

				// 4. now add to list of matrices
				//modelMatrices[i] = model;
				data[i].model = model;
				data[i].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

				data[i].model = model;


				data[i].color = glm::vec4(generate_random_glm_vec3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0)) * 10.0f * glm::vec3(0.002f, 0.7f, 0.5f), 1.0f);

			}
		}

	}



}

void f_init(Boxes_engine::Instance_data* data)
{
	float radius = 150.0;
	float offset = 25.0f;

	/*
	int num_cubes = 2000;
	std::vector<glm::vec3> colors_cubes(num_cubes);
	float factor = 1.0f / num_cubes;

	for (int i = 0; i < num_cubes; i++)
	{
		colors_cubes[i] = generate_random_glm_vec3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0)) * 10.0f * glm::vec3(0.002f, 0.7f, 0.5f);
	}*/


	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = static_cast<float>((rand() % 360));
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		data[i].model = model;
		if (i < amount * 0.5f)
		{
			data[i].color = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
		}
		else
		{
			data[i].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}

	}
}

void left_click()
{
	std::cout << "left click\n";
}

void left_relese()
{
	std::cout << "left relese\n";

	int frame_number = Boxes_engine::get_frame();
	Boxes_engine::captureAndSaveFrameBuffer(("C:/Users/Cosmos/Desktop/output/framebuffer_color_" + std::to_string(frame_number) + ".png").c_str());

}

void right_click()
{
	std::cout << "right click\n";
}

void right_relese()
{
	std::cout << "right relese\n";
}

void capture_framebuffer()
{
	
	int frame_number = Boxes_engine::get_frame();
	if(false)
	{
		std::cout << "Framebuffer capture \n";
		std::cout << "frame number : "<< frame_number <<" \n";
		Boxes_engine::captureAndSaveFrameBuffer(("C:/Users/Cosmos/Desktop/output/framebuffer_color_" + std::to_string(frame_number) + ".png").c_str());
	}
	
}

int main()
{
	std::cout << "BoxedInMotion\n";

	Boxes_engine::set_callback_mouse_button_left_click(left_click);
	Boxes_engine::set_callback_mouse_button_left_relese(left_relese);
	Boxes_engine::set_callback_mouse_button_right_click(right_click);
	Boxes_engine::set_callback_mouse_button_right_relese(right_relese);
	Boxes_engine::set_callback_on_finish_render_callback(capture_framebuffer);

	Boxes_engine::set_bloom_iteration(10);
	//Boxes_engine::play(amount, f_init, f_loop, 45.0f, 1000.0f);

	//camera_position
	//frame
	//time

	Boxes_engine::ShaderSourceCode source;

	source.fragment = 
		"#version 330 core\n"
		"out vec4 FragColor;\n"
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
		"	FragColor = Color + sin(time + FragWorldPos.x * 10.0) * 0.5 + vec4(vec3(sin(FragObjectPos.x * 100), 0.0, 0.0), 1.0);\n"
		"}\n"
		;

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
		"gl_Position = projection * view * instanceData.model * vec4(aPos + sin(time) * 10.0, 1.0f);\n"
		"}\n";

	source.geometry = nullptr;

	Boxes_engine::play(amount, f_init, f_loop, 45.0f, 1000.0f, &source);

	return 0;
}



/*


set camera all parameters


----

Than it comes I will create an github repository with the engine the engine needs a name than one thing that sould create is in youtube channel, gmail, instagram, tiktok where I will publish this videos ane link to the youtube channel and a patreon page


*/

/*
next version

than you should have a callback all keybord events use primitive build in data types seems usefull to triger certain actions on some key
find out if you can capture also the depth map may be usefull in post create a method that you can call to capture
*/

/*
DONE

there is the amount somewhere in the loop find out what it does and enable to set it's value maybe it may be usefull

try to get the world space and object space position in fragment shader of cube


send into the vertex shader and somehow to fragment shader the frame and t uniforms

add an option to override the default vertex and fragment shader

than you should add a optional callback after you render the scene that you can use to capture the framebuffer,

lock camera controls


void set_camera_position(glm::vec3 position);
this will enable by repositioning camera at origin 0,0,0 to to have infinite space we can explore

*/

// Graphic description
// rotating cube inside the cube there are some simple object like a smaller wireframe cube or ball hitting the walls and bounding of it
// while the object are leaving a train in 3d space
// the camera can be stationary and the big cube can be slowly rotating around
// the graphiy style should be glowing with wireframe vibe
