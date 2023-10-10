#pragma once

enum class StateType;
class StateManager;

class StateDependent {
public:
	StateDependent() : m_currentState((StateType)0) {}
	virtual ~StateDependent() {}
	virtual void CreateState(const StateType& l_state) {}
	virtual void ChangeState(const StateType& l_state) = 0;
	virtual void RemoveState(const StateType& l_state) = 0;

	StateType GetCurrentState()const { return m_currentState; }
protected:
	void SetState(const StateType& l_state) { m_currentState = l_state; }
	StateType m_currentState;
};