#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include "../Utilities/Utilities.h"

template<typename T>
using ResourcePair = std::pair<std::unique_ptr<T>, unsigned int>;
template<typename T>
using ResourceContainer = std::unordered_map<std::string, ResourcePair<T>>;
using PathContainer = std::unordered_map<std::string, std::string>;

template<typename Derived, typename T>
class ResourceManager{
public:
	ResourceManager(const std::string& l_pathsFile) {
		LoadPaths(l_pathsFile);
	}

	virtual ~ResourceManager() { PurgeResources(); }

	T* GetResource(const std::string& l_id) {
		sf::Lock lock(m_mutex);
		auto res = Find(l_id);
		return(res ? res->first.get() : nullptr);
	}

	std::string GetPath(const std::string& l_id) {
		sf::Lock lock(m_mutex);
		auto path = m_paths.find(l_id);
		return(path != m_paths.end() ? path->second : "");
	}

	bool RequireResource(const std::string& l_id) {
		sf::Lock lock(m_mutex);
		auto res = Find(l_id);
		if(res) {
			++res->second;
			return true;
		}
		auto path = m_paths.find(l_id);
		if (path == m_paths.end()) { return false; }
		{
			std::unique_ptr<T> resource = std::make_unique<T>();
			if (!Load(resource.get(), path->second)) { return false; }
			m_resources.emplace(l_id, std::make_pair(std::move(resource), 1));
		}
		return true;
	}

	bool ReleaseResource(const std::string& l_id) {
		sf::Lock lock(m_mutex);
		auto res = Find(l_id);
		if (!res) { return false; }
		--res->second;
		if (!res->second) { Unload(l_id); }
		return true;
	}

	void PurgeResources() {
		sf::Lock lock(m_mutex);
		std::cout << "Purging all resources:" << std::endl;
		while(m_resources.begin() != m_resources.end()) {
			std::cout << "Removing: " << m_resources.begin()->first << std::endl;
			m_resources.erase(m_resources.begin());
		}
		std::cout << "Purging finished." << std::endl;
	}
protected:
	bool Load(T* l_resource, const std::string& l_path) {
		return static_cast<Derived*>(this)->Load(l_resource, l_path);
	}
private:
	ResourcePair<T>* Find(const std::string& l_id) {
		auto itr = m_resources.find(l_id);
		return (itr != m_resources.end() ? &itr->second : nullptr);
	}

	bool Unload(const std::string& l_id) {
		auto itr = m_resources.find(l_id);
		if (itr == m_resources.end()) { return false; }
		m_resources.erase(itr);
		return true;
	}

	void LoadPaths(const std::string& l_pathFile) {
		std::ifstream paths;
		paths.open(Utils::GetWorkingDirectory() + l_pathFile);
		if(paths.is_open()) {
			std::string line;
			while(std::getline(paths,line)) {
				std::stringstream keystream(line);
				std::string pathName;
				std::string path;
				keystream >> pathName;
				keystream >> path;
				m_paths.emplace(pathName,path);
			}
			paths.close();
			return;
		}
		std::cerr << "! Failed loading the path file: " << l_pathFile << std::endl;
	}

	ResourceContainer<T> m_resources;
	PathContainer m_paths;

	sf::Mutex m_mutex;
};