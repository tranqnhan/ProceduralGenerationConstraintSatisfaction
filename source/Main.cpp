#include <raylib.h>

#include "SceneCoordinator.hpp"
#include "SceneSharedData.hpp"
#include "XorshiftRandom.hpp"

#include "Program.hpp"

uint32_t XorshiftRandom::randomState = 1234;

// Main loop initialization
void Init() {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(WINDOW_W, WINDOW_H, WINDOW_N);
    SetTargetFPS(60);
    HideCursor();

    SceneSharedData::Init();
    SceneCoordinator::Init();
}


// Main loop input
void Input() {
    SceneCoordinator::Input();
}


// Main loop update
void Update(float deltaTime) {
    SceneCoordinator::Update();
}


// Main loop draw
void Render() {
    BeginDrawing();
    ClearBackground(BLACK);

    SceneCoordinator::Render();

    EndDrawing();
}


// Main loop
int main(void) {
    Init();

    while (!WindowShouldClose()) {
        Input();
        Update(GetFrameTime());
        Render();
    }

    CloseWindow();
    return 0;
}