#include "app/app.h"

int main(void)
{
    App app = {0};

    if (!app_init(&app)) return 1;

    while (!app.should_quit) {
        app_handle_input(&app);
        app_update(&app);
        app_render(&app);
    }

    app_shutdown(&app);
    return 0;
}
