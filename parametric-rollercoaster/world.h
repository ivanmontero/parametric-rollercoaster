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
	GLuint pShader, aShader;

	// Math
	volatile bool new_func = false;
	bool init = false;
	std::thread t_func_input;
	std::mutex func_mutex; // Mutex for r, rp, and rpp
	std::array<Expr, 3> r;
	std::array<Expr, 3> rp;
	std::array<Expr, 3> rpp;
	// Params
	double min, max, step;
public:
	void Initialize();
	void Update(float delta);
	void Render();
	void Release();

	static World* GetInstance();
private:
	static World instance;
	//World& operator = (const World& other);
	World();
	World(World&);
};