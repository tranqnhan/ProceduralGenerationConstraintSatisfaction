#pragma once

#include "Scene.hpp"

#include "Generator.hpp"

class SceneGeneration : public Scene {
public:
    void OnEnter();
    void Input();
    void Update();
    void Render();
    void OnExit();

private:
    Generator tileGenerator;
};