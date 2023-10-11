#include "../BoxedInMotion/Engine.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>
#include <optional>

//#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/rapidjson.h"
#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/document.h"
#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/stringbuffer.h"
#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/writer.h"

#include <fstream>
#include <iostream>

#include <cstdlib>

struct Line3d
{
	glm::vec3 start;
	glm::vec3 end;
};

namespace Line3d_
{
	float length(const Line3d& line)
	{
		return glm::length(line.end - line.start);
	}

	glm::vec3 direction(const Line3d& line)
	{
		return glm::normalize(line.end - line.start);
	}

	glm::vec3 point_at(const Line3d& line, float t)
	{
		return line.start + glm::clamp(t, 0.0f, 1.0f) * (line.end - line.start);
	}

	std::optional<glm::vec3> Intersects(const Line3d& line, const Line3d& other)
	{
		glm::vec3 dir1 = line.end - line.start;
		glm::vec3 dir2 = other.end - other.start;
		glm::vec3 start2_start1 = other.start - line.start;

		glm::vec3 cross = glm::cross(dir1, dir2);
		if (glm::length(cross) < 1e-6) {
			// The lines are either parallel or collinear, so they don't intersect.
			return std::nullopt;
		}

		float t = glm::dot(glm::cross(start2_start1, dir2), cross) / glm::length(cross) / glm::length(cross);
		float s = glm::dot(glm::cross(start2_start1, dir1), cross) / glm::length(cross) / glm::length(cross);

		if (t >= 0.0f && t <= 1.0f && s >= 0.0f && s <= 1.0f) {
			// Calculate the intersection point.
			glm::vec3 intersection = line.start + t * dir1;
			return intersection;
		}

		return std::nullopt;
	}

	float distance_to_point(const Line3d& line, const glm::vec3& position)
	{
		glm::vec3 line_direction = line.end - line.start;
		glm::vec3 point_to_start = line.start - position;
		return glm::length(glm::cross(line_direction, point_to_start)) / glm::length(line_direction);
	}

	glm::vec3 midpoint(const Line3d& line)
	{
		return 0.5f * (line.start - line.end);
	}

	float angle_beteen(const Line3d& line, const Line3d& other)
	{
		glm::vec3 dir1 = glm::normalize(line.end - line.start);
		glm::vec3 dir2 = glm::normalize(other.end - other.start);
		return acos(glm::dot(dir1, dir2));
	}
}


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
	int generate_random_int(const int min_value, const int max_value)
	{
		return min_value + rand() % (max_value - min_value + 1);
	}

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
	struct DynamicLineSegment
	{
		Line3d line;
		glm::vec4 color;

		void init()
		{
			line.start = Utils::generate_random_glm_vec3(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f)) * 10.0f;
			line.end = Utils::generate_random_glm_vec3(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f)) * 10.0f;
			color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		void update(float dt, float t, float num)
		{

		}
	};

	std::vector<DynamicLineSegment> dls;
	

	void init(Engine::Instance_data* data)
	{
		for (int i = 0; i < 100; i++)
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
		for (int i = 0; i < 10000; i++)
		{
			index++;
			index = index % Constants::num_boxes;

			auto& model = data[index].model;
			model = glm::mat4(1.0f);


			int line_index = Utils::generate_random_int(0, dls.size());
			auto& ls = dls[line_index];
			ls.update(dt, t, i);
			glm::vec3 positon = Line3d_::point_at(ls.line, Utils::generate_random_float(0.0f, 1.0f));
			positon += Utils::generate_random_glm_vec3(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f)) * 0.1f;

			data[index].color = ls.color;

			model = glm::translate(model, positon);
			model = glm::scale(model, glm::vec3(0.01f));

			





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