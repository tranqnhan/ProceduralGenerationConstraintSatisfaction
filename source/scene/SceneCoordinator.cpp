#include <memory>

#include "SceneCoordinator.hpp"
#include "SceneGeneration.hpp"
#include "SceneTextureSample.hpp"

SceneId SceneCoordinator::currentSceneId;

std::vector<std::unique_ptr<Scene>> SceneCoordinator::scenes;

void SceneCoordinator::Init() {
    SceneCoordinator::scenes.resize(SceneId::NUM_SCENES);
    SceneCoordinator::AddScene(TextureSample, std::make_unique<SceneTextureSample>());
    //SceneCoordinator::AddScene(Generation, std::make_unique<SceneGeneration>());

    SceneCoordinator::currentSceneId = TextureSample;
    SceneCoordinator::scenes[SceneCoordinator::currentSceneId]->OnEnter();
}

void SceneCoordinator::Input() {
    SceneCoordinator::scenes[SceneCoordinator::currentSceneId]->Input();
}

void SceneCoordinator::Update() {
    SceneCoordinator::scenes[SceneCoordinator::currentSceneId]->Update();
}

void SceneCoordinator::Render() {
    SceneCoordinator::scenes[SceneCoordinator::currentSceneId]->Render();
}

void SceneCoordinator::ChangeScene(SceneId sceneId) {
    SceneCoordinator::scenes[SceneCoordinator::currentSceneId]->OnExit();
    SceneCoordinator::currentSceneId = sceneId;
    SceneCoordinator::scenes[SceneCoordinator::currentSceneId]->OnEnter();
}

void SceneCoordinator::AddScene(SceneId sceneId, std::unique_ptr<Scene>&& scene) {    
    SceneCoordinator::scenes[sceneId] = std::move(scene);
}
