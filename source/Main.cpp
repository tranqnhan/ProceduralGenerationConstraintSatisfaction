#include "raylib.h"

#include "Analyzer.hpp"

#define WINDOW_W 800
#define WINDOW_H 800
#define WINDOW_N "Procedural Generation Constraint Satisfaction"

Processor sampleAnalyzer;
//Generator generator;

// Main loop initialization
void Init() {
    InitWindow(WINDOW_W, WINDOW_H, WINDOW_N);
    SetTargetFPS(60);

    sampleAnalyzer.AnalyzeImage("../assets/sample1.png", 3);

//    generator.DebugInit(ruleset, 100, 100);
}


// Main loop update
void Update(float deltaTime) {
    //const int numIterations = 10;
    //for (int i = 0; i < numIterations; ++i) {
    //    generator.DebugNext();
    //}
}


// Main loop input
void Input() {
    //if (IsKeyPressed(KEY_SPACE)) {
    //    generator.DebugNext();
    //}
}


// Main loop draw
void Render() {
    BeginDrawing();
    ClearBackground(BLACK);

    //DrawFPS(0, 0);

    // Draw
    DrawTextureEx(sampleAnalyzer.debugTexture, Vector2{.x = 0, .y = 0}, 0, 16, WHITE);
    
    EndDrawing();
}


void OnClose() {
    //UnloadTexture(generator.debugTexture);
}

// Main loop
int main(void) {
    Init();

    while (!WindowShouldClose()) {
        Input();
        Update(GetFrameTime());
        Render();
    }

    OnClose();

    CloseWindow();
    return 0;
}