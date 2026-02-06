#include "kepler/kepler.h"

int main(void)
{
    if (!kepler_init()) return 1;

    while (!kepler_should_close()) {
        kepler_update();
        kepler_render();
    }

    kepler_shutdown();
    return 0;
}
