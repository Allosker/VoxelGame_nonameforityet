#include "world/world.hpp"

void World::World::update(const types::pos& camLoc)
{
	grid.update(camLoc);

	grid.draw_all();
}
