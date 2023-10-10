#include "FileManager.h"
#include "../GUI_Manager.h"
#include "../../StateSystem/StateManager.h"
#include "../../Utilities/Helpers.h"
#include "../../EventSystem/EventManager.h"

GUI_FileManager::GUI_FileManager(std::string l_name, GUI_Manager* l_guiMgr, StateManager* l_stateMgr)
	: m_guiManager(l_guiMgr), m_stateMgr(l_stateMgr), m_name(l_name), m_saveMode(false)
{
	m_guiManager->LoadInterface("FileManager.interface", l_name);
	m_interface = m_guiManager->GetInterface(l_name);
	m_currentState = m_stateMgr->GetCurrentStateType();
	m_folderEntry = m_interface->GetElement("FolderEntry")->GetStyleName();
	m_fileEntry = m_interface->GetElement("FileEntry")->GetStyleName();
	m_interface->RemoveElement("FolderEntry");
	m_interface->RemoveElement("FileEntry");
	m_interface->SetContentRectSize({ 300, 260 });
	m_interface->SetContentOffset({ 0.f, 16.f });
	m_interface->PositionCenterScreen();

	auto e = m_stateMgr->GetContext()->m_eventManager;

	e->AddCallback<GUI_FileManager>("FileManager_Parent", &GUI_FileManager::ParentDirCallback, this);
	e->AddCallback<GUI_FileManager>("FileManager_Entries", &GUI_FileManager::HandleEntries, this);
	e->AddCallback<GUI_FileManager>("FileManager_ActionButton", &GUI_FileManager::ActionButton, this);
	e->AddCallback<GUI_FileManager>("FileManager_Close", &GUI_FileManager::CloseButton, this);

	SetDirectory(Utils::GetWorkingDirectory());
}

GUI_FileManager::~GUI_FileManager() {
	m_guiManager->RemoveInterface(m_currentState, m_name);
	auto events = m_stateMgr->GetContext()->m_eventManager;
	events->RemoveCallback(m_currentState, "FileManager_Parent");
	events->RemoveCallback(m_currentState, "FileManager_Entries");
	events->RemoveCallback(m_currentState, "FileManager_ActionButton");
	events->RemoveCallback(m_currentState, "FileManager_Close");
}

void GUI_FileManager::HandleEntries(EventDetails* l_details) {
	if(l_details->m_guiElement.find("FEntry_") != std::string::npos) {
		std::string path = m_dir + m_interface->GetElement(l_details->m_guiElement)->GetText() + "\\";
		SetDirectory(path);
		m_interface->UpdateScrollVertical(0);
	} else if (l_details->m_guiElement.find("Entry_") != std::string::npos) {
		m_interface->GetElement("FileName")->SetText(m_interface->GetElement(l_details->m_guiElement)->GetText());
	}
}

void GUI_FileManager::ActionButton(EventDetails* l_details) {
	if (m_actionCallback == nullptr) { std::cout << "Action callback for file manager was not bound!" << std::endl; return; }
	auto filename = m_interface->GetElement("FileName")->GetText();
	m_actionCallback(m_dir + filename);
}

void GUI_FileManager::CloseButton(EventDetails* l_details) { Hide(); }

void GUI_FileManager::SetDirectory(std::string l_dir) {
	m_dir = l_dir;
	std::replace(m_dir.begin(), m_dir.end(), '\\', '/');
	m_interface->RemoveElementsContaining("Entry_");
	ListFiles();
}

void GUI_FileManager::ListFiles() {
	m_interface->GetElement("Directory")->SetText(m_dir);
	auto list = Utils::GetFileList(m_dir, "*.*", true);
	Utils::SortFileList(list);
	auto ParentDir = m_interface->GetElement("ParentDir");
	float x = ParentDir->GetPosition().x;
	float y = ParentDir->GetPosition().y + ParentDir->GetSize().y + 1.f;
	size_t i = 0;
	for (auto& file : list) {
		if (file.first == "." || file.first == "..") { continue; }
		std::string entry = (file.second ? "FEntry_" : "Entry_");
		m_interface->AddElement(GUI_ElementType::Label, entry + std::to_string(i));
		auto element = m_interface->GetElement(entry + std::to_string(i));
		element->SetText(file.first);
		element->SetPosition({ x, y });
		m_guiManager->LoadStyle((file.second ? m_folderEntry : m_fileEntry), element);
		y += ParentDir->GetSize().y + 4.f;
		++i;
	}
}

void GUI_FileManager::ParentDirCallback(EventDetails* l_details) {
	auto i = m_dir.find_last_of("/", m_dir.length() - 2);
	if (i != std::string::npos) {
		std::string dir = m_dir.substr(0U, i + 1);
		SetDirectory(dir);
	}
}

void GUI_FileManager::Hide() { m_interface->SetActive(false); }

void GUI_FileManager::Show() {
	m_interface->SetActive(true);
	m_interface->PositionCenterScreen();
	ListFiles();
	m_interface->Focus();
}

bool GUI_FileManager::IsInSaveMode() const { return m_saveMode; }
void GUI_FileManager::LoadMode() { m_interface->GetElement("ActionButton")->SetText("Load"); m_saveMode = false; }
void GUI_FileManager::SaveMode() { m_interface->GetElement("ActionButton")->SetText("Save"); m_saveMode = true; }