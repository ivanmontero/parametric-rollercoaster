#include <iostream>

#include "world.h"
#include "window.h"
#include "renderer.h"
#include "camera.h"
#include "mouse.h"
#include "keyboard.h"


// USE CONSOLE TO EXIT
void World::Initialize() {
	shader = Renderer::CreateShader("parametric.vert", "parametric.frag");

	// To allow consle input
	t_func_input = std::thread([this]() {
		std::array<std::string, 3> funcs = { "x(t)=", "y(t)=", "z(t)=" };
		while (!Window::ShouldClose()) {
			std::array<Expr, 3> new_r;
			for (int i = 0; i < funcs.size(); i++) {
				std::cout << funcs[i];
				std::string input;
				std::getline(std::cin, input); 
				if (Window::ShouldClose()) break;
				new_r[i] = Expr(input);
			}
			if (Window::ShouldClose()) break;
			new_func = true;
			std::lock_guard<std::mutex> lock(func_mutex);
			r = new_r;
			rp = { r[0].df("t"), r[1].df("t"), r[2].df("t") };
			rpp = { rp[0].df("t"), rp[1].df("t"), rp[2].df("t") };
			std::cout << r[0].to_string() << " " << r[1].to_string() << " " << r[2].to_string() << std::endl;
			std::cout << rp[0].to_string() << " " << rp[1].to_string() << " " << rp[2].to_string() << std::endl;
			std::cout << rpp[0].to_string() << " " << rpp[1].to_string() << " " << rpp[2].to_string() << std::endl;
		}
	});
}

void World::Update(float delta) {
	//std::cout << delta << std::endl;

	if (new_func) {
		// Generate new graph
		//std::cout << "entered! " << std::endl;
		// ...
		new_func = false;
		if (!init)
			init = true;
	}


	// Camera
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
	Renderer::SetShader(shader);
}

void World::Release() {
	// Show error 
	if (t_func_input.joinable()) {
		std::cout << std::string(100, '\n');
		for (int i = 0; i < 50; i++)
			std::cout << "\nPress enter to finalize exit procedure.";
		t_func_input.join();
	}
}

World::World() {}

World World::instance = World();

World* World::GetInstance() { return &instance; }