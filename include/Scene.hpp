#pragma once


class Scene {
public:
    virtual void OnEnter() = 0;
    virtual void Input() = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual void OnExit() = 0;

    virtual ~Scene() = default;
};
