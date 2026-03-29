#include <raylib.h>

#include "Program.hpp"
#include "Processor.hpp"
#include "Ruleset.hpp"
#include "SceneCoordinator.hpp"
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
}

void SceneGeneration::Input() {
    if (IsKeyDown(KEY_BACKSPACE)) {
        SceneCoordinator::ChangeScene(SceneId::TextureSample);
        return;
    }

    if (IsKeyDown(KEY_SPACE)) {
        this->tileGenerator.Next();
    }
}

void SceneGeneration::Update() {
    for (int i = 0; i < NUM_GENERATES_PER_ITERATION; ++i) {
        tileGenerator.Next();
    }
}

void SceneGeneration::Render() {
    tileGenerator.Render();
}


void SceneGeneration::OnExit() {

}
