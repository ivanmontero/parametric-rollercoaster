#include <iostream>

#include "world.h"
#include "window.h"
#include "renderer.h"
#include "camera.h"
#include "mouse.h"
#include "keyboard.h"
#include "mesh.h"



glm::vec3 World::to_opengl(glm::vec3 mathCoords) {
	return glm::vec3(mathCoords.y, mathCoords.z, mathCoords.x);
}

// USE CONSOLE TO EXIT
void World::Initialize() {
	aShader = Renderer::CreateShader("axes.vert", "axes.frag");
	cShader = Renderer::CreateShader("curve.vert", "curve.frag");

	Window::GrabCursor(true);
	Window::SetVSync(true);
	Camera::SetPosition(1.0f, 1.0f, 1.0f);
	Camera::SetRotation(PI / 4, -PI / 6, 0);
	Renderer::SetRenderDistance(1000.0f);

	// To allow console input
	t_func_input = std::thread([this]() {
		std::array<std::string, 6> prompts = { "x(t)=", "y(t)=", "z(t)=", "min t=", "max t=",  "step=" };
		while (!Window::ShouldClose()) {
			std::array<Expr, 3> new_r;
			std::array<double, 3> params;
			for (int i = 0; i < prompts.size(); i++) {
				std::cout << prompts[i];
				std::string in;
				std::getline(std::cin, in);
				if (Window::ShouldClose()) break;
				if (in.compare("q") == 0 || in.compare("quit") == 0) {
					Window::SetShouldClose(true);
					break;
				}
				if (i < 3)
					new_r[i] = Expr(in);
				else
					params[i - 3] = Expr::eval(in);
			}
			if (Window::ShouldClose()) break;

			new_func = true;
			std::lock_guard<std::mutex> lock(func_mutex);

			tmin = params[0];
			tstep = params[2];
			// Makes sure that the step is reasonable.
			tmax = floor((tmax - tmin) / tstep) * tstep;

			r = new_r;
			rp = { r[0].df("t"), r[1].df("t"), r[2].df("t") };
			rpp = { rp[0].df("t"), rp[1].df("t"), rp[2].df("t") };

			std::cout << "r(t)=<" << r[0].to_string() << ", " << r[1].to_string() << ", " << r[2].to_string() << ">\n";
			std::cout << "r'(t)=<" << rp[0].to_string() << ", " << rp[1].to_string() << ", " << rp[2].to_string() << ">\n";
			std::cout << "r''(t)=<" << rpp[0].to_string() << ", " << rpp[1].to_string() << ", " << rpp[2].to_string() << ">\n";

		}
	});
	// Create axes
	float axis_length = 50.0f;
	float ava[] = {
		0, 0, 0,
		axis_length, 0, 0,
		0, axis_length, 0,
		0, 0, axis_length
	};
	int aia[] = {
		0, 1,
		0, 2,
		0, 3
	};
	std::vector<Vertex> av;
	for (int i = 0; i < 4; i++) 
		av.push_back(Vertex(glm::vec3(ava[i * 3], ava[i * 3 + 1], ava[i * 3 + 2])));
	std::vector<GLuint> ai(std::begin(aia), std::end(aia));
	axes = new Mesh(av, ai);


}

void World::Update(float delta) {
	//std::cout << delta << std::endl;

	if (new_func) {
		// Generate new graph
		//std::cout << "entered! " << std::endl;
		// ...

		std::vector<Vertex> pv;
		for (float t = tmin; t <= tmax; t += tstep) {

		}




		new_func = false;
		if (!init)
			init = true;
	}


	// Camera
	float sensitivity = 0.0025f;
	while (Mouse::NextEvent()) {
		MouseEvent e = Mouse::Event;
		switch (e.type) {
		case BUTTON:
			//if (e.action == PRESS) {
			//	std::cout << "mouse pressed: " << e.button << " (" << Mouse::GetX() << ", " << Mouse::GetY() << ")" << std::endl;
			//}
			break;
		case CURSOR:
			Camera::RotatePitch(e.dy * sensitivity);
			Camera::RotateYaw(e.dx * sensitivity);

			if (Camera::GetPitch() > PI / 2.0f - 0.01f) Camera::SetPitch(PI / 2.0f - 0.01f);
			if (Camera::GetPitch() < -PI / 2.0f + 0.01f) Camera::SetPitch(-PI / 2.0f + 0.01f);
			break;
		}
	}

	float cameraSpeed = 10.0f * delta;
	if (Keyboard::IsKeyDown(KEY_W)) Camera::Move(Camera::GetFront() * cameraSpeed);
	if (Keyboard::IsKeyDown(KEY_S)) Camera::Move(-Camera::GetFront() * cameraSpeed);
	if (Keyboard::IsKeyDown(KEY_A)) Camera::Move(-Camera::GetRight() * cameraSpeed);
	if (Keyboard::IsKeyDown(KEY_D)) Camera::Move(Camera::GetRight() * cameraSpeed);
	if (Keyboard::IsKeyDown(KEY_SPACE)) Camera::Move(glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed);
	if (Keyboard::IsKeyDown(KEY_LEFT_SHIFT)) Camera::Move(glm::vec3(0.0f, -1.0f, 0.0f) * cameraSpeed);
}

void World::Render() {
	Renderer::Clear();

	// Axes
	Renderer::SetShader(aShader);
	Renderer::Render(axes, GL_LINES);
	
	// Graph
	if (init) {
		Renderer::SetShader(pShader);
		
	}

}

void World::Release() {
	if (t_func_input.joinable()) {
		std::cout << std::string(100, '\n');
		for (int i = 0; i < 50; i++)
			std::cout << "\nPress enter to finalize exit procedure.";
		t_func_input.join();
	}
}

World::World() {}

World::World(World&) {}

World World::instance = World();

//World& World::operator = (const World& other) {
//	std::lock(func_mutex, other.func_mutex);
//	std::lock_guard<std::mutex> self_lock(, std::adopt_lock);
//	std::lock_guard<std::mutex> other_lock(other.mtx, std::adopt_lock);
//	value = other.value;
//	return *this;
//}

World* World::GetInstance() { return &instance; }