#pragma once

#include "raylib.h"

#include "Scene.hpp"

class SceneTextureSample : public Scene {
public:
    SceneTextureSample();
    
    void OnEnter();
    void Input();
    void Update();
    void Render();
    void OnExit();
    
private:
    Texture2D sampleTexture;
    Color colors[10];
};