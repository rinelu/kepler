#include "render_world.h"

#include <assert.h>
#include "../world/world.h"
#include "celestial_render.h"

void render_world(const World* world, Camera camera)
{
    celestial_render_world(world, camera);
}

void render_world_reload(void)
{
}
