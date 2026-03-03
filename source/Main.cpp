#include "Generator.hpp"
#include "raylib.h"

#include "Analyzer.hpp"

#define WINDOW_W 800
#define WINDOW_H 800
#define WINDOW_N "Procedural Generation Constraint Satisfaction"

Analyzer sampleAnalyzer;
Generator generator;

Texture2D generatedTexture;

// Main loop initialization
void Init() {
    InitWindow(WINDOW_W, WINDOW_H, WINDOW_N);
    SetTargetFPS(60);


    Ruleset ruleset = sampleAnalyzer.AnalyzeImage("../assets/sample2.png");


    Image generatedImage = generator.GenerateImage(ruleset, 100, 100);
    UnloadImage(generatedImage);

    generatedTexture = LoadTextureFromImage(generatedImage);
}


// Main loop update
void Update(float deltaTime) {

}


// Main loop input
void Input() {

}


// Main loop draw
void Render() {
    BeginDrawing();
    ClearBackground(BLACK);

    //DrawFPS(0, 0);

    // Draw
    DrawTextureEx(generatedTexture, Vector2{.x = 0, .y = 0}, 0, 8, WHITE);

    EndDrawing();
}


void OnClose() {
    UnloadTexture(generatedTexture);
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