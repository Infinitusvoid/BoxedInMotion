#pragma once

namespace Random
{
	int generate_int(const int min_value, const int max_value)
	{
		return min_value + rand() % (max_value - min_value + 1);
	}

	float generate_float(const float min_value, const float max_value)
	{
		return min_value + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value - min_value);
	}

	glm::vec3 generate_glm_vec3(const glm::vec3& min_value, const glm::vec3& max_value)
	{
		glm::vec3 result;
		result.x = min_value.x + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value.x - min_value.x);
		result.y = min_value.y + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value.y - min_value.y);
		result.z = min_value.z + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value.z - min_value.z);
		return result;
	}

	glm::vec3 generate_glm_vec3_inside_cube_one_minus_one()
	{
		return Random::generate_glm_vec3(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	}

}