#pragma once


#include <string>

struct Scene_info
{
	int num_per_iteration_loop; // 10000
};

void init(Scene_info& scene_info, std::string set_file_path);