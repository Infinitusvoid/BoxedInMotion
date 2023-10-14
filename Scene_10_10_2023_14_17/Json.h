#pragma once

//#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/rapidjson.h"
#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/document.h"
#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/stringbuffer.h"
#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/writer.h"


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