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
	constexpr int num_boxes_x = 256;
	constexpr int num_boxes_y = 256;
	constexpr int num_boxes_z = 16;
	constexpr int num_boxes_layer = num_boxes_x * num_boxes_y;

	constexpr int num_boxes = num_boxes_x * num_boxes_y * num_boxes_z;


}

void calcualte_local_2d_axis(const Line3d& line, glm::vec3* out_axis_x, glm::vec3* out_axis_y)
{
	GLM_Utils::calculate_local_2d_axis(line.start, line.end, out_axis_x, out_axis_y);
}




namespace Scene_
{
	struct DynamicColorLine
	{
		Line3d line;
		glm::vec4 color;

		void init()
		{
			line.start = Random::generate_glm_vec3_inside_cube_one_minus_one() * 10.0f;
			line.end = Random::generate_glm_vec3_inside_cube_one_minus_one() * 10.0f;
			color = glm::vec4(1.0f, 1.01f, 1.001f, 1.0f);
		}

		void update(float dt, float t, float num)
		{
			color = glm::vec4(1.0f, 1.01f, 1.001f, 1.0f);
		}
	};

	struct Index
	{
		Index() :
			index{ -1 }
		{

		}

		int next()
		{
			index++;
			index = index % Constants::num_boxes;
			return index;
		}

	private:
		int index;
	};

	struct Boxes
	{
		Index index;

		void update(Engine::Instance_data* data, float t,float dt, std::vector<DynamicColorLine> dls)
		{
			for (int i = 0; i < g_scene_info.num_per_iteration_loop; i++)
			{
				int box_index = this->index.next();

				auto& model = data[box_index].model;
				model = glm::mat4(1.0f);


				int line_index = Random::generate_int(0, dls.size());
				
				auto& ls = dls[line_index];
				ls.update(dt, t, i);
				glm::vec3 positon = Line3d_::point_at(ls.line, Random::generate_float(0.0f, 1.0f));
				positon += Random::generate_glm_vec3(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f)) * 0.024f;

				data[box_index].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);//ls.color;

				model = glm::translate(model, positon);
				model = glm::scale(model, glm::vec3(0.002f));
			}
		}

		
	};
	
	struct Scene
	{
		std::vector<DynamicColorLine> dls;
		Boxes boxes;
		bool first_time = true;


		void update_dcl(float t, float dt)
		{
			if (first_time)
			{
				first_time = false;

				{
					dls.clear();

					glm::vec3 v0 = glm::vec3(0.0f, -10.0f, 0.0f);
					glm::vec3 v1 = glm::vec3(20.0f, 10.0f, 0.0f);

					DynamicColorLine dl_0;
					dl_0.line.start = v0;
					dl_0.line.end = v1;
					dl_0.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
					dls.push_back(dl_0);


					int num_steps = 24;
					float factor = 1.0f / static_cast<float>(num_steps);

					glm::vec3 axis_x;
					glm::vec3 axis_y;
					calcualte_local_2d_axis(dl_0.line, &axis_x, &axis_y);

					for (int i = 0; i < num_steps; i++)
					{
						glm::vec3 offset = Line3d_::point_at(dl_0.line, i * factor);

						{
							DynamicColorLine dl_n;
							dl_n.line.start = offset + axis_x * 0.0f + axis_y * 0.0f;
							dl_n.line.end = offset + axis_x * 0.0f + axis_y * 1.0f;
							dl_n.color = glm::vec4(0.9f, 1.0f, 1.0f, 1.0f);
							dls.push_back(dl_n);
						}

						{
							DynamicColorLine dl_n;
							dl_n.line.start = offset + axis_x * 0.0f + axis_y * 1.0f;
							dl_n.line.end = offset + axis_x * 1.0f + axis_y * 1.0f;
							dl_n.color = glm::vec4(0.9f, 1.0f, 1.0f, 1.0f);
							dls.push_back(dl_n);
						}

						{
							DynamicColorLine dl_n;
							dl_n.line.start = offset + axis_x * 1.0f + axis_y * 1.0f;
							dl_n.line.end = offset + axis_x * 1.0f + axis_y * 0.0f;
							dl_n.color = glm::vec4(0.9f, 1.0f, 1.0f, 1.0f);
							dls.push_back(dl_n);
						}

						{
							DynamicColorLine dl_n;
							dl_n.line.start = offset + axis_x * 1.0f + axis_y * 0.0f;
							dl_n.line.end = offset + axis_x * 0.0f + axis_y * 0.0f;
							dl_n.color = glm::vec4(0.9f, 1.0f, 1.0f, 1.0f);
							dls.push_back(dl_n);
						}
					}

				}
			}
			else
			{
				return;
			}

			



		}

		void update(Engine::Instance_data* data, float t, float dt)
		{

			//update_dcl(t, dt);
			//boxes.update(data, t, dt, dls);
			
		}
	};

	Scene scene;

	void init(Engine::Instance_data* data)
	{
		

		for (unsigned int i = 0; i < Constants::num_boxes; i++)
		{
			data[i].model = glm::mat4(1.0f);
			auto& model = data[i].model;
			model = glm::mat4(1.0f);
			
			
			int index_x = float(i % Constants::num_boxes_x);
			int index_y = float(((i / Constants::num_boxes_y) % Constants::num_boxes_y));
			int index_z = float(i / Constants::num_boxes_layer);
			
			if (true)
			{
				glm::vec3 offset = glm::vec3(
					round(index_x),
					round(index_y),
					round(index_z)
				) * 0.0f;
				model = glm::translate(model, offset);
			}
			
			
			//model = glm::translate(model, glm::vec3(1, 1, 1));
			model = glm::scale(model, glm::vec3(0.5f));
			

			auto& color = data[i].color;
			color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);


		}
	}

	void loop(Engine::Instance_data* data)
	{
		float dt = Engine::get_dt();
		float t = Engine::get_total_time();
		
		scene.update(data, t, dt);
		
	}

}




int main()
{
	std::cout << "Staring : Scene_25_10_2023_14_42\n";

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

	
	return 0;
}