#pragma once


#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include <iostream>
#include <cstdlib>

#include "Utils_GLM.h"
#include "Line3d.h"
#include "Json.h"
#include "Random.h"




struct Scene
{
public:
	static struct Constants
	{
		static constexpr int num_boxes = 1000 * 1000;
		static constexpr int num_per_iteration_loop = 10 * 1000;
	};

	
private:
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

	struct Boxes
	{
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

		Index index;

		void update(Engine::Instance_data* data, float t, float dt, std::vector<DynamicColorLine> dls)
		{
			for (int i = 0; i < Constants::num_per_iteration_loop; i++)
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

public:
	std::vector<DynamicColorLine> dls;
	Boxes boxes;
	bool first_time = true;

private:
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
				Line3d_::calcualte_local_2d_axis(dl_0.line, &axis_x, &axis_y);

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

public:
	void update(Engine::Instance_data* data, float t, float dt)
	{

		//update_dcl(t, dt);
		//boxes.update(data, t, dt, dls);

	}
};


