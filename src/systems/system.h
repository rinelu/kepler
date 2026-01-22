typedef struct App App;

typedef struct System {
    const char* name;
    void (*init)(App* app);
    void (*update)(App* app, float dt);
    void (*shutdown)(App* app);
} System;
