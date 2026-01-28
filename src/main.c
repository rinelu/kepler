#include "app/app.h"

int main(void)
{
    if (!app_init()) return 1;

    while (!app_should_close()) {
        app_handle_input();
        app_update();
        app_render();
    }

    app_shutdown();
    return 0;
}
