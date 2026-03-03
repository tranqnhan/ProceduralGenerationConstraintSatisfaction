#pragma once

#include <cstdint>
#include <vector>

#include "raylib.h"

#include "Heap.hpp"
#include "Ruleset.hpp"

class Generator {
public:
    Generator();

    Image GenerateImage(const Ruleset& rule, int width, int height);

    // prng with xorshift from low to high inclusive
    int RandomInteger(int lower, int upper) {
        randomState ^= randomState << 13;
        randomState ^= randomState >> 17;
        randomState ^= randomState << 5;

        return lower + randomState % (upper - lower + 1);
    }

    // TODO: for debug purposes
    Image debugImage;
    Texture2D debugTexture;
    Ruleset debugRules;
    int debugWidth; 
    int debugHeight;

    std::vector<bool> debugExplored;
    std::vector<uint32_t> debugConstraints;
    Heap<int> debugOpenSet = Heap<int>([this](const int& a, const int& b) -> bool {
        return std::popcount(this->debugConstraints[a]) > std::popcount(this->debugConstraints[b]);
    }); // stores index of next cells to solve

    void DebugInit(const Ruleset& rules, int width, int height);
    void DebugNext();


private:
    uint32_t randomState;

    int GetEntropy(uint32_t constraints) const;
    uint32_t AddConstraints(uint32_t oldContraints, uint32_t newConstraints) const;
    uint32_t ResolveContraints(uint32_t contraints); // Reduce contraints to 1 solution

};
