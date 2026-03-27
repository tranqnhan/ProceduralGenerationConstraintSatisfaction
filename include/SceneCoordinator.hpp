#pragma once

#include <memory>
#include <vector>

#include "Scene.hpp"

enum SceneId {
    TextureSample = 0,
    Generation = 1,
    NUM_SCENES
};


class SceneCoordinator {
public:
    static void Init();
    static void Input();
    static void Update();
    static void Render();
    static void ChangeScene(SceneId sceneId);
    
private:
    static std::vector<std::unique_ptr<Scene>> scenes;
    static SceneId currentSceneId;

    static void AddScene(SceneId sceneId, std::unique_ptr<Scene>&& scene);
};