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

	struct ShaderSourceCode
	{
		const char* vertex;
		const char* fragment;
		const char* geometry;
	};

	void set_background_color(glm::vec3 background_color);

	void set_exposure(float value);

	void set_bloom_iteration(int num_iterations);

	float get_dt();
	float get_total_time();
	int get_frame();

	glm::vec3 get_camera_position();
	glm::vec3 get_camera_front();
	glm::vec3 get_camera_right();
	glm::vec3 get_camera_up();
	float get_camera_zoom();
	void set_camera_parameters(glm::vec3 set_position, float set_speed, float set_sensitivy, float set_zoom);

	void set_camera_position(glm::vec3 position);
	void lock_camera();
	void unlock_camera();

	int play(unsigned int number_of_boxes, void(*f_init)(Instance_data*), void(*f_loop)(Instance_data*), float fov = 45.0f, float view_distance = 1000.0f, ShaderSourceCode* source = nullptr);
	
	void set_callback_mouse_button_left_click(void(*f)(void));
	void set_callback_mouse_button_left_relese(void(*f)(void));
	void set_callback_mouse_button_right_click(void(*f)(void));
	void set_callback_mouse_button_right_relese(void(*f)(void));

	void set_callback_on_finish_render_callback(void(*f)(void));
	void captureAndSaveFrameBuffer(const char* path);

	

}