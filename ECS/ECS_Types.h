#pragma once
using ComponentType = unsigned int;

enum class Component{
	Position = 0, SpriteSheet, State, Movable, Controller, Collidable, SoundEmitter, SoundListener, COUNT
};

enum class System{
	Renderer = 0, Movement, Collision, Control, State, SheetAnimation, Sound, COUNT
};