#include "Scene_info.h"

#include "Json_writer.h"

#include <fstream>
#include <iostream>
#include <filesystem>

static std::string file_path = "";

static bool no_file()
{
	return !std::filesystem::exists(file_path);
}

static void write()
{
	if (!std::filesystem::exists(file_path))
	{
		Json_writer json_write;
		json_write.Key("num_per_iteration_loop");
		json_write.Int(10 * 1000);
		json_write.WriteToFile(file_path);
	}
}

static void read(Scene_info& scene)
{
	std::ifstream inFile(file_path);
	if (inFile.is_open())
	{
		std::string json((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

		rapidjson::Document document;
		document.Parse(json.c_str());

		if (!document.HasParseError())
		{


			// Extract and print data as before
			if (document.HasMember("name") && document["name"].IsString())
			{
				std::string name = document["name"].GetString();
				std::cout << "Name: " << name << std::endl;
			}

			if (document.HasMember("nelixij") && document["nelixij"].IsInt())
			{
				int neon = document["nelixij"].GetInt();
				std::cout << "nelixij: " << neon << std::endl;
			}

			if (document.HasMember("num_per_iteration_loop") && document["num_per_iteration_loop"].IsInt())
			{
				scene.num_per_iteration_loop = document["num_per_iteration_loop"].GetInt();
				std::cout << "scene.num_per_iteration_loop " << scene.num_per_iteration_loop << std::endl;
			}

			if (document.HasMember("isStudent") && document["isStudent"].IsBool())
			{
				bool is_student = document["isStudent"].GetBool();
				std::cout << "is a student " << is_student << "\n";
			}


			if (document.HasMember("grades") && document["grades"].IsArray())
			{
				auto grades = document["grades"].GetArray();
				for (int i = 0; i < grades.Size(); i++)
				{
					double grade = grades[i].GetDouble();
					std::cout << "grade : " << grade << "\n";
				}
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

void init(Scene_info& scene_info, std::string set_file_path)
{
	file_path = set_file_path;
	if (no_file())
	{
		write();
	}

	read(scene_info);
}
