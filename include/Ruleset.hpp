#pragma once

#define MAX_DISTINCT_OBJECTS 32 // Maximum number of distinct objects to constraint

#include <cstdint>
#include <array>

#include "raylib.h"


class Ruleset {
public:

    Ruleset();

    void AddColor(const Color& color);
    Color GetColor(int id) const; 
    void AddConstraint(int id, int other, int direction);
    uint32_t GetConstraints(int id, int direction) const;
    int GetNumberOfObjects() const;

private:
    int numObjects;
    std::array<Color, MAX_DISTINCT_OBJECTS> colors;
    std::array<uint32_t, MAX_DISTINCT_OBJECTS * 8> constraints;

};
