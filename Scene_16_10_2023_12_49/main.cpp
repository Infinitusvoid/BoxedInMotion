#include "../BoxedInMotion/Engine.h"

#include <iostream>
#include <string>
#include <vector>
#include <optional>


#include <fstream>
#include <iostream>
#include <filesystem>
#include <cstdlib>

#include "GLM_Utils.h"
#include "Line3d.h"
#include "Random.h"



#include "Scene_info.h"

Scene_info g_scene_info;

namespace Constants
{
	constexpr int num_boxes = 1000 * 1000;
}


namespace Scene_
{
	void init(Engine::Instance_data* data)
	{
		for (unsigned int i = 0; i < Constants::num_boxes; i++)
		{
			data[i].model = glm::mat4(1.0f);
			auto& model = data[i].model;
			model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(i, 0, 0));
			//model = glm::translate(model, glm::vec3(1, 1, 1));
			model = glm::scale(model, glm::vec3(0.01f));


			auto& color = data[i].color;
			color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);


		}
	}

	void loop(Engine::Instance_data* data)
	{
		float dt = Engine::get_dt();
		float t = Engine::get_total_time();
	}

}




int main()
{
	//Json::Json json;
	//json.init();

	init(g_scene_info, "info.json");
	

	Engine::set_background_color(glm::vec3(0.0004f, 0.002f, 0.0017f));
	//Engine::set_bloom_iteration(20);
	// variations
	Engine::set_bloom_iteration(20);
	//Engine::set_bloom_iteration(74);
	//Engine::set_bloom_iteration(42);
	//Engine::set_bloom_iteration(24);
	//Engine::set_bloom_iteration(10);
	//Engine::set_bloom_iteration(4);

	//Engine::ShaderSourceCode source;		

	Engine::set_camera_parameters(glm::vec3(0.0, 0.0, 32.0), 10.0f, 0.2f, 45.0f);
	//Engine::play(num_boxes, init, loop, 45.0f, 1000.0f, &source);
	Engine::play(Constants::num_boxes, Scene_::init, Scene_::loop, 45.0f, 1000.0f);

	std::cout << "Scene_10_10_2023_14_17\n";
	return 0;
}