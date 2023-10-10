#include <fstream>
#include "FileLoader.h"
#include "../Utilities/Utilities.h"
#include <SFML/Graphics.hpp>

FileLoader::FileLoader() : m_totalLines(0), m_currentLine(0) {}
void FileLoader::AddFile(const std::string& l_file) { m_files.emplace_back(l_file, 0); }
size_t FileLoader::GetTotalLines() { sf::Lock lock(m_mutex); return m_totalLines; }
size_t FileLoader::GetCurrentLine() { sf::Lock lock(m_mutex); return m_currentLine; }
void FileLoader::SaveToFile(const std::string& l_file) {}
void FileLoader::ResetForNextFile() {}

void FileLoader::Work() {
	CountFileLines();
	if (!m_totalLines) { std::cout << "No lines to process." << std::endl; Done(); return; }
	for (auto& path : m_files) {
		ResetForNextFile();
		std::ifstream file(path.first);
		std::string line;
		std::string name;
		auto linesLeft = path.second;
		while (std::getline(file, line)) {
			{
				sf::Lock lock(m_mutex);
				++m_currentLine;
				--linesLeft;
			}
			if (line[0] == '|') { continue; }
			std::stringstream keystream(line);
			if (!ProcessLine(keystream)) {
				std::cout << "File loader terminated due to an internal error." << std::endl;
				{
					sf::Lock lock(m_mutex);
					m_currentLine += linesLeft;
				}
				break;
			}
		}
		file.close();
	}
	m_files.clear();
	Done();
}

void FileLoader::CountFileLines() {
	m_totalLines = 0;
	m_currentLine = 0;
	for (auto path = m_files.begin(); path != m_files.end();) {
		if (path->first.empty()) { path = m_files.erase(path); continue; }
		std::ifstream file(path->first);
		if (!file.is_open()) {
			std::cerr << "Failed to load file: " << path->first << std::endl;
			path = m_files.erase(path);
			continue;
		}
		file.unsetf(std::ios_base::skipws);
		{
			sf::Lock lock(m_mutex);
			path->second = static_cast<size_t>(std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n'));
			m_totalLines += path->second;
			std::cout << "Lines in file '" << path->first << "': " << path->second << "\n";
		}
		++path;
		file.close();
	}
}