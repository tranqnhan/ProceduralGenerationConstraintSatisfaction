#include "Cursor.hpp"
#include <raylib.h>


Cursor::Cursor() {}


void Cursor::Init() {
    this->cursorTexture = LoadTexture("assets/cursor.png");
}


void Cursor::Input() {
    this->positionX = GetMouseX();
    this->positionY = GetMouseY();
}


void Cursor::Update() {}


void Cursor::Render() {
    DrawTextureEx(this->cursorTexture, 
        Vector2 {
            .x=(float)(this->positionX - ((float)this->cursorTexture.width)), 
            .y=(float)(this->positionY - ((float)this->cursorTexture.height)),
        },
        0,
        2,
        WHITE);
}


Cursor::~Cursor() {
    UnloadTexture(this->cursorTexture);
}