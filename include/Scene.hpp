#pragma once


class Scene {
public:
    virtual void Init();
    virtual void Input();
    virtual void Update();
    virtual void Render();
};
