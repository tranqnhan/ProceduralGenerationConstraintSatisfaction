#include "Processor.hpp"
#include "Program.hpp"
#include "Ruleset.hpp"
#include "SceneSharedData.hpp"
#include "SceneGeneration.hpp"


void SceneGeneration::OnEnter() {
    Processor sampleProcessor;
    Ruleset ruleset = sampleProcessor.AnalyzeImage(
        (Color *)SceneSharedData::sampleImage.data,
        SceneSharedData::sampleImage.width, 
        SceneSharedData::sampleImage.height, 
        EXPAND);

    this->tileGenerator.Init(ruleset, CHUNK_WIDTH, CHUNK_HEIGHT, NUM_CHUNKS_WIDTH, NUM_CHUNKS_HEIGHT);
    this->uiTileDisplay.Init();
    this->uiCursor.Init();
}

void SceneGeneration::Input() {
    if (IsKeyDown(KEY_SPACE)) {
        this->tileGenerator.Next();
    }
    this->uiTileDisplay.Input();
    this->uiCursor.Input();
}

void SceneGeneration::Update() {
    const int numIterations = 10;
    for (int i = 0; i < numIterations; ++i) {
        tileGenerator.Next();
    }

    uiTileDisplay.Update(tileGenerator);
    uiCursor.Update();
}

void SceneGeneration::Render() {
    tileGenerator.Render();

    uiTileDisplay.Render();
    uiCursor.Render();
}


void SceneGeneration::OnExit() {

}
