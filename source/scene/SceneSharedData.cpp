#include <raylib.h>

#include "Program.hpp"
#include "SceneSharedData.hpp"

Image SceneSharedData::sampleImage;

void SceneSharedData::Init() {
    SceneSharedData::sampleImage = GenImageColor(SAMPLE_WIDTH, SAMPLE_HEIGHT, WHITE);
}