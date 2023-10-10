#include "TileSet.h"

TileSet::TileSet(TextureManager* l_textures) : m_textureMgr(l_textures) {}
TileSet::~TileSet() { FreeTexture(); }
TileSetContainer& TileSet::GetContainer() { return m_tileSet; }

void TileSet::FreeTexture() {
	if (m_textureName.empty()) { return; }
	m_textureMgr->ReleaseResource(m_textureName);
	m_textureName = "";
}

bool TileSet::ProcessLine(std::stringstream& l_stream) {
	std::string type;
	l_stream >> type;
	if (type == "Texture") {
		if (!m_textureName.empty()) { return true; }
		l_stream >> m_textureName;
		m_textureMgr->RequireResource(m_textureName);
		return true;
	}
	if (m_textureName.empty()) { return false; }
	int tileId = std::stoi(type);
	if (tileId < 0) { return true; }
	auto tile = std::make_unique<TileInfo>(m_textureMgr->GetResource(m_textureName), tileId);
	l_stream >> tile->m_name >> tile->m_friction.x >> tile->m_friction.y >> tile->m_deadly;
	if (!m_tileSet.emplace(tileId, std::move(tile)).second) {
		// Duplicate tile detected!
		std::cout << "! Duplicate tile type: " << tile->m_name << std::endl;
		tile = nullptr;
	}
	return true;
}

void TileSet::Purge() { m_tileSet.clear(); FreeTexture(); }
std::string TileSet::GetTextureName() const { return m_textureName; }
void TileSet::SetName(const std::string& l_name) { m_name = l_name; }
std::string TileSet::GetName() const { return m_name; }