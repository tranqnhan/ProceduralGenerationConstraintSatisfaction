#pragma once

#include "Scene.hpp"

#include "Generator.hpp"
#include "Cursor.hpp"
#include "TileDisplay.hpp"

class SceneGeneration : public Scene {
public:
    void OnEnter();
    void Input();
    void Update();
    void Render();
    void OnExit();

private:
    Generator tileGenerator;

    Cursor uiCursor;
    TileDisplay uiTileDisplay;

};