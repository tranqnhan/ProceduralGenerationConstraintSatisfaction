#include "Ruleset.hpp"

Ruleset::Ruleset() {
    numObjects = 0;
    constraints.fill(0);
}

void Ruleset::AddColor(const Color& color){
    if (numObjects >= MAX_DISTINCT_OBJECTS) return;
    colors[numObjects++] = color;
}

void Ruleset::AddConstraint(int id, int other, int direction) {
    constraints[id * 8 + direction] |= (1 << other);
}

uint32_t Ruleset::GetConstraints(int id, int direction) const {
    return constraints[id * 8 + direction];
}

int Ruleset::GetNumberOfObjects() const {
    return numObjects;
}

Color Ruleset::GetColor(int id) const {
    if (id >= numObjects) return colors[0];
    return colors[id];
} 
