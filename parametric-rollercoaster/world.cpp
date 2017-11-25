#include "world.h"

void World::Initialize() {
}

void World::Update(float delta) {
}

void World::Render() {
}

void World::Release() {
}

World World::instance = World();

World* World::GetInstance() { return &instance; }

World::World() {}