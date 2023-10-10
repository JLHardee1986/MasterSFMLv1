#pragma once
#include <string>
#include <functional>

class GUI_Manager;
class GUI_Interface;
class StateManager;
struct EventDetails;
enum class StateType;

class GUI_FileManager {
public:
	GUI_FileManager(std::string l_name, GUI_Manager* l_guiMgr, StateManager* l_stateMgr);
	~GUI_FileManager();

	void SetDirectory(std::string l_dir);

	void ParentDirCallback(EventDetails* l_details);
	void HandleEntries(EventDetails* l_details);
	void ActionButton(EventDetails* l_details);
	void CloseButton(EventDetails* l_details);

	void Hide();
	void Show();
	void LoadMode();
	void SaveMode();

	bool IsInSaveMode() const;

	template<class T>
	void SetActionCallback(void(T::*l_method)(const std::string&), T* l_instance) {
		m_actionCallback = [l_instance, l_method](const std::string& l_str) -> void
		{ (l_instance->*l_method)(l_str); };
	}
private:
	void ListFiles();
	GUI_Interface* m_interface;
	std::string m_name;
	std::string m_dir;

	std::string m_folderEntry;
	std::string m_fileEntry;

	GUI_Manager* m_guiManager;
	StateManager* m_stateMgr;
	StateType m_currentState;
	std::function<void(std::string)> m_actionCallback;
	bool m_saveMode;
};