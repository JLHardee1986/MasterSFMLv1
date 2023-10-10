#pragma once
#include <sstream>
#include "ParticleContainer.h"

class Emitter;

class BaseGenerator {
public:
	virtual ~BaseGenerator() {}
	virtual void Generate(Emitter* l_emitter, ParticleContainer* l_particles, size_t l_from, size_t l_to) = 0;
	friend std::stringstream& operator >> (
		std::stringstream& l_stream, BaseGenerator& b)
	{
		b.ReadIn(l_stream);
		return l_stream;
	}

	virtual void ReadIn(std::stringstream& l_stream) {}
};