#include "../BoxedInMotion/Engine.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>
#include <optional>


#include <fstream>
#include <iostream>

#include <cstdlib>

#include "GLM_Utils.h"


#include "Line3d.h"
#include "Json.h"

#include "Random.h"




namespace Constants
{
	const int num_boxes = 1200 * 1000;
}


namespace Scene
{
	struct DynamicLineSegment
	{
		Line3d line;
		glm::vec4 color;

		void init()
		{
			line.start = glm::vec3(0.0f, 0.0f, 0.0f);//Utils::generate_random_glm_vec3(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f)) * 10.0f;
			line.end = glm::vec3(0.0f, 0.0f, 0.0f);//Utils::generate_random_glm_vec3(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f)) * 10.0f;
			color = glm::vec4(1.0f, 1.01f, 1.001f, 1.0f);
		}

		void update(float dt, float t, float num)
		{

		}

		

	};

	std::vector<DynamicLineSegment> dls;


	
	

	void calcualte_local_2d_axis(const Line3d& line, glm::vec3* out_axis_x, glm::vec3* out_axis_y)
	{
		GLM_Utils::calculate_local_2d_axis(line.start, line.end, out_axis_x, out_axis_y);
	}

	void update_DynamicLineSegments_l(std::vector<DynamicLineSegment>& l, float t, float dt)
	{
		l.clear();

		glm::vec3 v0 = glm::vec3(0.0f, -10.0f, 0.0f);
		glm::vec3 v1 = glm::vec3(20.0f, 10.0f, 0.0f);
		
		DynamicLineSegment dl_0;
		dl_0.line.start = v0;
		dl_0.line.end = v1;
		dl_0.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		l.push_back(dl_0);

		
		int num_steps = 24;
		float factor = 1.0f / static_cast<float>(num_steps);

		glm::vec3 axis_x;
		glm::vec3 axis_y;
		calcualte_local_2d_axis(dl_0.line, &axis_x, &axis_y);

		for (int i = 0; i < num_steps; i++)
		{
			glm::vec3 offset = Line3d_::point_at(dl_0.line, i * factor);

			{
				DynamicLineSegment dl_n;
				dl_n.line.start = offset + axis_x * 0.0f + axis_y * 0.0f;
				dl_n.line.end = offset + axis_x * 0.0f + axis_y * 1.0f;
				dl_n.color = glm::vec4(0.9f, 1.0f, 1.0f, 1.0f);
				l.push_back(dl_n);
			}

			{
				DynamicLineSegment dl_n;
				dl_n.line.start = offset + axis_x * 0.0f + axis_y * 1.0f;
				dl_n.line.end = offset + axis_x * 1.0f + axis_y * 1.0f;
				dl_n.color = glm::vec4(0.9f, 1.0f, 1.0f, 1.0f);
				l.push_back(dl_n);
			}

			{
				DynamicLineSegment dl_n;
				dl_n.line.start = offset + axis_x * 1.0f + axis_y * 1.0f;
				dl_n.line.end = offset + axis_x * 1.0f + axis_y * 0.0f;
				dl_n.color = glm::vec4(0.9f, 1.0f, 1.0f, 1.0f);
				l.push_back(dl_n);
			}

			{
				DynamicLineSegment dl_n;
				dl_n.line.start = offset + axis_x * 1.0f + axis_y * 0.0f;
				dl_n.line.end = offset + axis_x * 0.0f + axis_y * 0.0f;
				dl_n.color = glm::vec4(0.9f, 1.0f, 1.0f, 1.0f);
				l.push_back(dl_n);
			}
		}
		

	}

	void update_DynamicLineSegments(std::vector<DynamicLineSegment>& l, float t, float dt)
	{
		int num_segments = l.size();

		int draw_what = Random::generate_int(0, 100);
		
		update_DynamicLineSegments_l(l, t, dt);
		return;

		//glm::vec3 v0 = Utils::generate_random_glm_vec3(glm::vec3(-1.0, -1.0, -1.0), glm::vec3(1.0f, 1.0f, 1.0f));
		//v0 *= 0.1f;
		//v0 += glm::vec3(0.0, -10.0f, 0.0f);

		//glm::vec3 v1 = Utils::generate_random_glm_vec3(glm::vec3(-1.0, -1.0, -1.0), glm::vec3(1.0f, 1.0f, 1.0f));
		//v1 *= 0.1f;
		//v1 += glm::vec3(10.0f, 10.0f, 0.0f);

		glm::vec3 v0 = glm::vec3(0.0f,  -10.0f, 0.0f);
		glm::vec3 v1 = glm::vec3(20.0f, 10.0f, 0.0f);



		auto& lt = l[Random::generate_int(0, num_segments)];


		if (draw_what < 50)
		{
			
			lt.line.start = v0;
			lt.line.end = v1;
		}
		else
		{
			Line3d line = Line3d(v0, v1);
			glm::vec3 vcmp = Line3d_::midpoint(line);
			//glm::vec3 vcmp = Line3d_::point_at(line, 0.5f);
			
			
			//lt.line.start = vcmp;
			//lt.line.end = vcmp + v_0 * glm::length(vcmp);
			

			
			glm::vec3 t0 = vcmp;
			glm::vec3 t1 = glm::normalize(glm::cross(line.end - line.start, glm::vec3(0.0f, 1.0f, 0.0f)));
			glm::vec3 t2 = vcmp + t1 * Line3d_::length(line);
			glm::vec3 t2_ = vcmp + glm::normalize(glm::cross(t2, line.start - line.end)) * Line3d_::length(line);


			lt.line.start = t0;
			lt.line.end = t2_;
			
			lt.color = glm::vec4(1.0f, 2.0f, 1.1f, 1.0f);
		}


	}
	

	void init(Engine::Instance_data* data)
	{
		for (int i = 0; i < 1000; i++)
		{
			dls.emplace_back(DynamicLineSegment());
			dls[i].init();
		}


		for (unsigned int i = 0; i < Constants::num_boxes; i++)
		{
			data[i].model = glm::mat4(1.0f);
			auto& model = data[i].model;
			model = glm::mat4(1.0f);
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
		float t = Engine::get_total_time();

		update_DynamicLineSegments(dls, t, dt);

		for (int i = 0; i < 10000; i++)
		{
			index++;
			index = index % Constants::num_boxes;

			auto& model = data[index].model;
			model = glm::mat4(1.0f);


			int line_index = Random::generate_int(0, dls.size());
			auto& ls = dls[line_index];
			ls.update(dt, t, i);
			glm::vec3 positon = Line3d_::point_at(ls.line, Random::generate_float(0.0f, 1.0f));
			positon += Random::generate_glm_vec3(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f)) * 0.024f;

			data[index].color = ls.color;

			model = glm::translate(model, positon);
			model = glm::scale(model, glm::vec3(0.002f));

			





		}
	}

}





int main()
{
	Json::Json json;
	json.init();

	Engine::set_background_color(glm::vec3(0.0004f, 0.002f, 0.0017f));
	//Engine::set_bloom_iteration(20);
	// variations
	Engine::set_bloom_iteration(100);
	//Engine::set_bloom_iteration(74);
	//Engine::set_bloom_iteration(42);
	//Engine::set_bloom_iteration(24);
	//Engine::set_bloom_iteration(10);
	//Engine::set_bloom_iteration(4);

	//Engine::ShaderSourceCode source;


	

	
		

	//Engine::set_camera_parameters(glm::vec3(0.0, 0.0, 0.0), 10.0f, 0.2f, 45.0f);
	//Engine::play(num_boxes, init, loop, 45.0f, 1000.0f, &source);
	Engine::play(Constants::num_boxes, Scene::init, Scene::loop, 45.0f, 1000.0f);

	std::cout << "Scene_10_10_2023_14_17\n";
	return 0;
}