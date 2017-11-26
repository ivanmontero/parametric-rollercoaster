#pragma once
#include <string>
#include <vector>
#include <string>
#include <vector>
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>

// Modified to work with only positions

#define POSITION glm::vec3

struct Vertex {
	POSITION Position;
	Vertex(POSITION pos) : Position(pos) {}
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);

	GLuint VAO, VBO, EBO;
private:
	void SetUp();
};