#include "Json_writer.h"

#include <iostream>
#include <fstream>



Json_writer::Json_writer() :
	writer(buffer)
{
	writer.StartObject();
}

void Json_writer::Key(const std::string& key)
{
	writer.Key(key.c_str());
}

void Json_writer::String(const std::string& value)
{
	writer.String(value.c_str());
}

void Json_writer::Int(int value)
{
	writer.Int(value);
}

void Json_writer::Double(double value)
{
	writer.Double(value);
}

void Json_writer::Bool(bool value)
{
	writer.Bool(value);
}

void Json_writer::StartObject()
{
	writer.StartObject();
}

void Json_writer::EndObject()
{
	writer.EndObject();
}

void Json_writer::StartArray()
{
	writer.StartArray();
}

void Json_writer::EndArray()
{
	writer.EndArray();
}

bool Json_writer::WriteToFile(const std::string& filename)
{
	writer.EndObject();  // Close the root object
	std::ofstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Failed to open file for writing: " << filename << std::endl;
		return false;
	}
	file << buffer.GetString();
	file.close();
	return true;
}

std::string Json_writer::ToString()
{
	writer.EndObject();  // Close the root object
	return buffer.GetString();
}
