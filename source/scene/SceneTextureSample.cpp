#include <raylib.h>

#include "SceneTextureSample.hpp"
#include "SceneSharedData.hpp"


SceneTextureSample::SceneTextureSample() {
    this->sampleTexture = LoadTextureFromImage(SceneSharedData::sampleImage);

    constexpr int numColors = 10;

    unsigned int colorHex[numColors] = {
        0x639bffff,
        0xac3232ff,
        0x76428aff,
        0xfbf236ff,
        0x6abe30ff,
        0x37946eff,
        0xdf7126ff,
        0xac3232ff,
        0x8a6f30ff,
        0x663931ff,
    };

    for (int i = 0; i < numColors; ++i) {
        this->colors[i] = GetColor(colorHex[i]);
    }
}


void SceneTextureSample::OnEnter() {
    
}


void SceneTextureSample::Input() {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        const int x = GetMouseX();
        const int y = GetMouseY();
        
    }
}


void SceneTextureSample::Update() {

}


void SceneTextureSample::Render() {

}



void SceneTextureSample::OnExit() {
    
}