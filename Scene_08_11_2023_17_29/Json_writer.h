#pragma once

#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/rapidjson.h"
#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/stringbuffer.h"
#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/writer.h"
#include "../External_libs/rapidjson/rapidjson-master/rapidjson-master/include/rapidjson/document.h"

#include <string>

struct Json_writer
{
	Json_writer();

	void Key(const std::string& key);

	void String(const std::string& value);

	void Int(int value);

	void Double(double value);

	void Bool(bool value);

	void StartObject();

	void EndObject();

	void StartArray();

	void EndArray();

	bool WriteToFile(const std::string& filename);

	std::string ToString();

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer;
};