#pragma once
#include "scene.h"

class World : public Scene {
public:
	void Initialize();
	void Update(float delta);
	void Render();
	void Release();

	static World* GetInstance();
private:
	static World instance;
	World();
};