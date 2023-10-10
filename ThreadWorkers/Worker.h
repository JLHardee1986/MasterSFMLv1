#pragma once
#include <SFML/System/Thread.hpp>

class Worker {
public:
	Worker() : m_thread(&Worker::Work, this), m_done(false), m_started(false) {}
	virtual void OnAdd() {}
	virtual void OnRemove() {}
	void Begin() {
		sf::Lock lock(m_mutex);
		if(m_done || m_started) { return; }
		m_started = true;
		m_thread.launch();
	}
	bool IsDone() { sf::Lock lock(m_mutex); return m_done; }
	bool HasStarted() { sf::Lock lock(m_mutex); return m_started; }
	void ResetWorker() { sf::Lock lock(m_mutex); m_done = false; m_started = false; }
protected:
	void Done() { m_done = true; }
	virtual void Work() = 0;
	sf::Thread m_thread;
	sf::Mutex m_mutex;
	bool m_done;
	bool m_started;
};