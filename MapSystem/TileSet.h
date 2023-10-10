#pragma once
#include <unordered_map>
#include <memory>
#include "TileInfo.h"
#include "../ThreadWorkers/FileLoader.h"

using TileSetContainer = std::unordered_map<TileID, std::unique_ptr<TileInfo>>;

class TileSet : public FileLoader {
public:
	TileSet(TextureManager* l_textures);
	~TileSet();
	TileSetContainer& GetContainer();

	void Purge();
	std::string GetTextureName() const;
	void SetName(const std::string& l_name);
	std::string GetName()const;
private:
	void FreeTexture();
	bool ProcessLine(std::stringstream& l_stream);
	TileSetContainer m_tileSet;
	TextureManager* m_textureMgr;

	std::string m_textureName;
	std::string m_name;
};