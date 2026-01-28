#pragma once
#include <stdbool.h>

void relaxation_init(void);
void relaxation_update(float dt);
bool relaxation_active(void);
