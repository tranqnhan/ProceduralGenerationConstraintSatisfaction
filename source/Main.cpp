#include <chrono>
#include <iostream>

#include <raylib.h>

#include "XorshiftRandom.hpp"
#include "Processor.hpp"
#include "Generator.hpp"
#include "TileDisplay.hpp"

#include "Program.hpp"

uint32_t XorshiftRandom::randomState = 1000;

// Tile generator
Generator tileGenerator;

// UI
TileDisplay uiTileDisplay;

// Main loop initialization
void Init() {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(WINDOW_W, WINDOW_H, WINDOW_N);
    SetTargetFPS(60);
    HideCursor();

    Processor sampleProcessor;

    auto start = std::chrono::steady_clock::now(); 

    Ruleset ruleset = sampleProcessor.AnalyzeImage("../assets/sample11.png", EXPAND);

    auto stop = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Analyze image time: " << duration.count() << " microseconds" << std::endl;
    
    tileGenerator.Init(ruleset, CHUNK_WIDTH, CHUNK_HEIGHT, NUM_CHUNKS_WIDTH, NUM_CHUNKS_HEIGHT);
    uiTileDisplay.Init();

}


// Main loop input
void Input() {
    if (IsKeyDown(KEY_SPACE)) {
        tileGenerator.Next();
    }
    uiTileDisplay.Input();
}


// Main loop update
void Update(float deltaTime) {
    const int numIterations = 10;
    for (int i = 0; i < numIterations; ++i) {
        tileGenerator.Next();
    }

    uiTileDisplay.Update(tileGenerator);
}


// Main loop draw
void Render() {
    BeginDrawing();
    ClearBackground(BLACK);

    tileGenerator.Render();

    // UI Draw
    uiTileDisplay.Render();

   // DrawFPS(0, 0);

    EndDrawing();
}


void OnClose() {
    
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