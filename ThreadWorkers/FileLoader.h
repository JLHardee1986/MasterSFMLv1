#pragma once
#include <string>
#include <vector>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Mutex.hpp>
#include "Worker.h"

using LoaderPaths = std::vector<std::pair<std::string, size_t>>;

class FileLoader : public Worker {
public:
	FileLoader();
	void AddFile(const std::string& l_file);
	virtual void SaveToFile(const std::string& l_file);

	size_t GetTotalLines();
	size_t GetCurrentLine();
private:
	virtual bool ProcessLine(std::stringstream& l_stream) = 0;
	virtual void ResetForNextFile();
	void Work();
	void CountFileLines();

	LoaderPaths m_files;
	size_t m_totalLines;
	size_t m_currentLine;
};