#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <thread>
#include <array>
#include <mutex>
#include "scene.h"

#include "expr.h"

class World : public Scene {
private:
	// Rendering
	GLuint shader;

	// Math
	volatile bool new_func = false;
	bool init = false;
	std::thread t_func_input;
	std::mutex func_mutex; // Mutex for r, rp, and rpp
	std::array<Expr, 3> r;
	std::array<Expr, 3> rp;
	std::array<Expr, 3> rpp;
public:
	void Initialize();
	void Update(float delta);
	void Render();
	void Release();

	static World* GetInstance();
private:
	static World instance;
	World();
	World(World&);
	
};