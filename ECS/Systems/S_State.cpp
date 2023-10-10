#include "S_State.h"
#include "../Core/System_Manager.h"
#include "../../Types/Directions.h"

S_State::S_State(SystemManager* l_systemMgr)
	: S_Base(System::State,l_systemMgr)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::State);
	m_requiredComponents.push_back(req);

	m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Move,this);
	m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Switch_State,this);
}

S_State::~S_State() {}

void S_State::Update(float l_dT) {
	EntityManager* entities = m_systemManager->GetEntityManager();
	for(auto &entity : m_entities) {
		auto state = entities->GetComponent<C_State>(entity, Component::State);
		if(state->GetState() == EntityState::Walking) {
			Message msg((MessageType)EntityMessage::Is_Moving);
			msg.m_receiver = entity;
			m_systemManager->GetMessageHandler()->Dispatch(msg);
		}
	}
}

void S_State::HandleEvent(const EntityId& l_entity,
	const EntityEvent& l_event)
{
	switch(l_event) {
	case EntityEvent::Became_Idle:
		ChangeState(l_entity,EntityState::Idle,false);
		break;
	}
}

void S_State::Notify(const Message& l_message) {
	if (!HasEntity(l_message.m_receiver)) { return; }
	EntityMessage m = static_cast<EntityMessage>(l_message.m_type);
	switch(m) {
	case EntityMessage::Move:
		{
			auto state = m_systemManager->GetEntityManager()->GetComponent<C_State>(l_message.m_receiver,Component::State);

			if (state->GetState() == EntityState::Dying) { return; }
			EntityEvent e;
			Direction dir = static_cast<Direction>(l_message.m_int);
			if (dir == Direction::Up) { e = EntityEvent::Moving_Up; }
			else if (dir == Direction::Down) { e = EntityEvent::Moving_Down; }
			else if(dir == Direction::Left) { e = EntityEvent::Moving_Left; }
			else if (dir == Direction::Right) { e = EntityEvent::Moving_Right; }

			m_systemManager->AddEvent(l_message.m_receiver, static_cast<EventID>(e));
			ChangeState(l_message.m_receiver,EntityState::Walking,false);
		}
		break;
	case EntityMessage::Switch_State: 
		ChangeState(l_message.m_receiver, (EntityState)l_message.m_int,false);
		break;
	}
}

void S_State::ChangeState(const EntityId& l_entity, 
	const EntityState& l_state, bool l_force)
{
	EntityManager* entities = m_systemManager->GetEntityManager();
	auto state = entities->GetComponent<C_State>(l_entity, Component::State);
	if (!l_force && state->GetState() == EntityState::Dying) { return; }
	state->SetState(l_state);
	Message msg((MessageType)EntityMessage::State_Changed);
	msg.m_receiver = l_entity;
	msg.m_int = static_cast<int>(l_state);
	m_systemManager->GetMessageHandler()->Dispatch(msg);
}