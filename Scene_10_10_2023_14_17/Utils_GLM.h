#pragma once

#include "glm/glm.hpp"

namespace Utils
{
	namespace GLM 
	{
		static constexpr glm::vec3 constant_vec3_up = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::vec3 direction(const glm::vec3 & start, const glm::vec3 & end)
		{
			return glm::normalize(end - start);
		}


		void calculate_local_2d_axis(const glm::vec3 & start, const glm::vec3 & end, glm::vec3 * out_axis_x, glm::vec3 * out_axis_y)
		{
			if (start == end)
			{
				// Handle the degenerate case appropriately, e.g., by setting the axes to default values.
				*out_axis_x = glm::vec3(1.0f, 0.0f, 0.0f);
				*out_axis_y = glm::vec3(0.0f, 1.0f, 0.0f);
				return;
			}

			glm::vec3 line_direction = direction(start, end);

			*out_axis_x = glm::normalize(glm::cross(constant_vec3_up, line_direction));
			*out_axis_y = glm::normalize(glm::cross(*out_axis_x, line_direction));
		}
	}
}


