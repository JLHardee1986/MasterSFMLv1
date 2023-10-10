#pragma once
#include <sstream>
#include <string>

class MapLoadee {
public:
	virtual void ReadMapLine(const std::string& l_type, std::stringstream& l_stream) = 0;
	virtual void SaveMap(std::ofstream& l_fileStream) = 0;
};