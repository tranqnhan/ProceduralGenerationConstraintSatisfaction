#pragma once

#include <vector>

#include "Scene.hpp"

class SceneCoordinator {
public:
    static void Init();
    static void Input();
    static void Update();
    static void Render();

private:
    static std::vector<Scene> scenes;
    static int currentSceneId;
};