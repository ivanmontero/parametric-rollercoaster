#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <thread>
#include <array>
#include <mutex>
#include "expr.h"
#include "glm/glm.hpp"
#include "scene.h"

class Mesh;

// Parametric Curve Vertex
struct CVertex {
	float t;
	glm::vec3 position;
	glm::vec3 tangent;
	glm::vec3 normal;
	glm::vec3 binormal;
};

class World : public Scene {
private:
	// Axes
	GLuint aShader;
	Mesh* axes;
	// Parametric Curve
	GLuint cShader;
	Mesh* curve;
	std::vector<CVertex> cVertices;
	// Math
	volatile bool new_func = false;
	bool init = false;
	std::thread t_func_input;
	std::mutex func_mutex; // Mutex for r, rp, and rpp
	std::array<Expr, 3> r;
	std::array<Expr, 3> rp;
	std::array<Expr, 3> rpp;
	// Params
	double tmin, tmax, tstep;

public:
	glm::vec3 to_opengl(glm::vec3 mathCoords);




	// Boilerplate
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