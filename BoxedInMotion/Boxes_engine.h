#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Boxes_engine
{
	struct Instance_data
	{
		glm::mat4 model;
		glm::vec4 color;
	};

	void set_background_color(glm::vec3 background_color);

	void set_exposure(float value);

	float get_dt();
	float get_total_time();
	int get_frame();

	glm::vec3 get_camera_position();
	glm::vec3 get_camera_front();
	glm::vec3 get_camera_right();
	glm::vec3 get_camera_up();
	float get_camera_zoom();

	void set_camera_position(glm::vec3 position);

	int play(unsigned int number_of_boxes, void(*f_init)(Instance_data*), void(*f_loop)(Instance_data*), float fov = 45.0f, float view_distance = 1000.0f);

	void set_callback_mouse_button_left_click(void(*f)(void));
	void set_callback_mouse_button_left_relese(void(*f)(void));
	void set_callback_mouse_button_right_click(void(*f)(void));
	void set_callback_mouse_button_right_relese(void(*f)(void));
}