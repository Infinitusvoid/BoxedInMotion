#include "../BoxedInMotion/Engine.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>

//#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/rapidjson.h"
#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/document.h"
#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/stringbuffer.h"
#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/writer.h"

#include <fstream>
#include <iostream>



namespace Json
{
	// Define a function to write the JSON parsing example to a file
	void WriteToJsonFile(const std::string& filename) {
		const char* json = "{\"name\":\"John\",\"age\":30,\"city\":\"New York\"}";

		rapidjson::Document document;
		document.Parse(json);

		if (!document.HasParseError()) {
			std::ofstream outFile(filename);
			if (outFile.is_open()) {
				rapidjson::StringBuffer buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				document.Accept(writer);
				outFile << buffer.GetString();
				outFile.close();
			}
			else {
				std::cerr << "Failed to open the file for writing." << std::endl;
			}
		}
		else {
			std::cerr << "Failed to parse JSON." << std::endl;
		}
	}

	// Define a function to read the JSON data from a file
	void ReadFromJsonFile(const std::string& filename) {
		std::ifstream inFile(filename);
		if (inFile.is_open()) {
			std::string json((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

			rapidjson::Document document;
			document.Parse(json.c_str());

			if (!document.HasParseError()) {
				// Extract and print data as before
				if (document.HasMember("name") && document["name"].IsString()) {
					std::string name = document["name"].GetString();
					std::cout << "Name: " << name << std::endl;
				}
				if (document.HasMember("age") && document["age"].IsInt()) {
					int age = document["age"].GetInt();
					std::cout << "Age: " << age << std::endl;
				}
				if (document.HasMember("city") && document["city"].IsString()) {
					std::string city = document["city"].GetString();
					std::cout << "City: " << city << std::endl;
				}
			}
			else {
				std::cerr << "Failed to parse JSON from the file." << std::endl;
			}
			inFile.close();
		}
		else {
			std::cerr << "Failed to open the file for reading." << std::endl;
		}
	}

	// Define a wrapper class to abstract the process
	class JSONFileHandler {
	public:
		JSONFileHandler(const std::string& filename) : filename_(filename) {}

		void WriteToFile() {
			WriteToJsonFile(filename_);
		}

		void ReadFromFile() {
			ReadFromJsonFile(filename_);
		}

	private:
		std::string filename_;
	};

	struct Json
	{
		

		void init()
		{
			// Define the filename
			const std::string filename = "data.json";

			// Use the wrapper class to write and read JSON data
			JSONFileHandler jsonHandler(filename);
			jsonHandler.WriteToFile();
			jsonHandler.ReadFromFile();


			if (!file_exist())
			{
				// write default  file
				{

				}
			}
			
			// load from file
			{

			}
			
		}

		bool file_exist()
		{
			return false;
		}

		
	};
}

namespace Utils
{
	float generate_random_float(const float min_value, const float max_value)
	{
		return min_value + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value - min_value);
	}

	glm::vec3 generate_random_glm_vec3(const glm::vec3& min_value, const glm::vec3& max_value)
	{
		glm::vec3 result;
		result.x = min_value.x + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value.x - min_value.x);
		result.y = min_value.y + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value.y - min_value.y);
		result.z = min_value.z + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max_value.z - min_value.z);
		return result;
	}

}

namespace Constants
{
	const int num_boxes = 1200 * 1000;
}


namespace Scene
{
	struct Agent
	{
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
		static constexpr float box_side = 2.0f;
		static constexpr float half_box_side = box_side * 0.5f;
		static constexpr float the_velocity_scaling = 0.1f;

		void init()
		{

			position = Utils::generate_random_glm_vec3(glm::vec3(-half_box_side, -half_box_side, -half_box_side), glm::vec3(half_box_side, half_box_side, half_box_side));


			velocity = glm::normalize(Utils::generate_random_glm_vec3(glm::vec3(-1.0, -1.0f, -1.0), glm::vec3(1.0f, 1.0f, 1.0f))) * the_velocity_scaling;

			color *= 1.1f;
		}



		void update(const float dt, const float t, const float id)
		{


			position += velocity;

			float fak_0 = 0.9;
			float fak_1 = 1.0f - fak_0;

			float color_fak = 4.0f;
			float color_fak2 = 0.5f;


			if (Utils::generate_random_float(0.0f, 1.0f) > 0.5f)
			{
				if (glm::length(position) > half_box_side)
				{

					float fak = 1.0 + 0.5f * sinf(t * 0.1f);
					float fak2 = 1.0f - fak;

					float value = sin(position.y * 10.0f + t * 0.2f) * fak + sin(position.y * 24.0f - t * 0.47f) * fak2;

					glm::vec4 color_lerped = glm::mix(glm::vec4(0.0f, 0.2f, 0.2f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), value);

					position = glm::normalize(position) * half_box_side * (0.76f + 0.24f * value);
					velocity *= 0.9;
					color = glm::vec4(Utils::generate_random_float(0.0f, 0.5f), Utils::generate_random_float(0.0f, 1.0f), Utils::generate_random_float(0.0f, 1.0f), 1.0f);
					color *= color_fak;
					color *= color_lerped;
				}
				else
				{
					position = Utils::generate_random_glm_vec3(glm::vec3(-half_box_side, -half_box_side, -half_box_side), glm::vec3(half_box_side, half_box_side, half_box_side));
					velocity = glm::normalize(Utils::generate_random_glm_vec3(glm::vec3(-1.0, -1.0f, -1.0), glm::vec3(1.0f, 1.0f, 1.0f))) * the_velocity_scaling;
				}
			}


			glm::vec4 color_dark_green = glm::vec4(0.05, 0.5, 0.2, 1.0);
			glm::vec4 color_light_green = glm::vec4(0.2, 2.7, 0.5, 1.0);

			glm::vec4 color_dark_v2 = glm::vec4(0.05, 0.5, 1.2, 1.0);
			glm::vec4 color_light_v2 = glm::vec4(0.2, 2.7, 3.5, 1.0);

			if (glm::length(position) < half_box_side)
			{
				velocity += glm::normalize(position - glm::vec3(0.0f, 0.0f, 0.0f)) * 10.0f;
				velocity += glm::normalize(Utils::generate_random_glm_vec3(glm::vec3(-1.0, -1.0f, -1.0), glm::vec3(1.0f, 1.0f, 1.0f))) * 0.1f;
				velocity = glm::normalize(velocity);
				color = glm::mix(color_dark_green, color_light_green, sin(glm::length(position) * 10.0f + t));
			}
			else
			{
				velocity -= glm::normalize(position - glm::vec3(0.0f, 0.0f, 0.0f)) * 10.0f;
				velocity += glm::normalize(Utils::generate_random_glm_vec3(glm::vec3(-1.0, -1.0f, -1.0), glm::vec3(1.0f, 1.0f, 1.0f))) * 0.1f;
				velocity = glm::normalize(velocity);
				color = glm::mix(color_dark_v2, color_light_v2, sin(glm::length(position) * 10.0f - t * 2.2f));
			}


		}
	};

	std::vector<Agent> agents;

	void init(Engine::Instance_data* data)
	{
		for (int i = 0; i < Constants::num_boxes; i++)
		{
			agents.emplace_back(Agent());
			agents[i].init();
		}


		for (unsigned int i = 0; i < Constants::num_boxes; i++)
		{
			data[i].model = glm::mat4(1.0f);
			auto& model = data[i].model;
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
		for (int i = 0; i < 10000; i++)
		{
			index++;
			index = index % Constants::num_boxes;

			auto& model = data[index].model;
			model = glm::mat4(1.0f);

			agents[index].update(dt, t, i);


			data[index].color = agents[index].color;


			model = glm::translate(model, agents[index].position);
			model = glm::scale(model, glm::vec3(0.0004f) * (1.2f + 0.4f * sinf(i * 0.001f + t)));






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