#include <iostream>

#include "world.h"
#include "window.h"
#include "renderer.h"
#include "camera.h"
#include "mouse.h"
#include "keyboard.h"



// USE CONSOLE TO EXIT
void World::Initialize() {
	aShader = Renderer::CreateShader("axes.vert", "axes.frag");
	pShader = Renderer::CreateShader("parametric.vert", "parametric.frag");

	// To allow console input
	t_func_input = std::thread([this]() {
		std::array<std::string, 6> prompts = { "x(t)=", "y(t)=", "z(t)=", "min t=", "max t=",  "step="};
		while (!Window::ShouldClose()) {
			std::array<Expr, 3> new_r;
			std::array<double, 3> params;
			for (int i = 0; i < prompts.size(); i++) {
				std::cout << prompts[i];
				std::string in;
				std::getline(std::cin, in); 
				if (Window::ShouldClose()) break;
				if (i < 3)
					new_r[i] = Expr(in);
				else
					params[i - 3] = Expr::eval(in);
			}
			if (Window::ShouldClose()) break;

			new_func = true;
			std::lock_guard<std::mutex> lock(func_mutex);

			min = params[0];
			step = params[2];
			// Makes sure that the step is reasonable.
			max = floor((max - min) / step) * step;

			r = new_r;
			rp = { r[0].df("t"), r[1].df("t"), r[2].df("t") };
			rpp = { rp[0].df("t"), rp[1].df("t"), rp[2].df("t") };

			std::cout << "r(t)=<" << r[0].to_string() << ", " << r[1].to_string() << ", " << r[2].to_string() << ">\n";
			std::cout << "r'(t)=<" << rp[0].to_string() << ", " << rp[1].to_string() << ", " << rp[2].to_string() << ">\n";
			std::cout << "r''(t)=<" << rpp[0].to_string() << ", " << rpp[1].to_string() << ", " << rpp[2].to_string() << ">\n";

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
	Renderer::SetShader(pShader);

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