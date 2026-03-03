#include <cstdint>
#include <bit>
#include <vector>

#include "Heap.hpp"
#include "raylib.h"

#include "Ruleset.hpp"
#include "Generator.hpp"


Generator::Generator() {
    randomState = 12345; //std::time(0);
}

int Generator::GetEntropy(uint32_t constraints) const {
    return std::popcount(constraints);
}

uint32_t Generator::AddConstraints(uint32_t oldContraints, uint32_t newConstraints) const {
    return oldContraints & newConstraints;
}


uint32_t Generator::ResolveContraints(uint32_t contraints) {
    int totalPossibilities = std::popcount(contraints);

    if (totalPossibilities == 0) return 0;

    int pick = RandomInteger(1, totalPossibilities);

    uint32_t mask = 0b1;

    while (pick > 0) {     
        if (mask & contraints) {
            pick--;
        }
        mask <<= 1;
    }

    mask >>= 1;

    return mask & contraints;
}


Image Generator::GenerateImage(const Ruleset& rules, int width, int height) {
    
    std::vector<bool> explored(width * height, false);
    std::vector<uint32_t> constraints(width * height, ~((~0) << rules.GetNumberOfObjects()));
    
    int startingPoint = RandomInteger(0, width * height - 1);

    Heap<int> openSet([constraints](const int& a, const int& b) -> bool {
        return std::popcount(constraints[a]) > std::popcount(constraints[b]);
    }); // stores index of next cells to solve
    
    // TODO: since item and unique id is the same, optimize the heap to reflect this
    openSet.Push(startingPoint, startingPoint);

    // Constraint satisfaction
    while (openSet.GetSize() > 0) {
        const int cellIndex = openSet.TopItemID();
        openSet.Pop();
        
        constraints[cellIndex] = ResolveContraints(constraints[cellIndex]);
        
        if (constraints[cellIndex] == 0) continue;

        const int type = std::countr_zero(constraints[cellIndex]);
        explored[cellIndex] = true;

        // Get neighbors
        const int cellX = cellIndex % width;
        const int cellY = cellIndex / height;

        const int boundMinY = (cellY - 1) > 0 ? (cellY - 1) : 0;
        const int boundMinX = (cellX - 1) > 0 ? (cellX - 1) : 0;
        const int boundMaxY = (cellY + 2) > height ? height : (cellY + 2);
        const int boundMaxX = (cellX + 2) > width ? width : (cellX + 2);
        
        int direction = -1;
        for (int i = boundMinY; i < boundMaxY; ++i) {
            for (int j = boundMinX; j < boundMaxX; ++j) {
                if (i == cellY && j == cellX) continue;
                direction++;
                const int nextIndex = i * width + j;
                if (explored[nextIndex]) continue;
                constraints[nextIndex] = AddConstraints(constraints[nextIndex], rules.GetConstraints(type, direction));
                openSet.Push(nextIndex, nextIndex);
            }
        }
    }


    // Generate image
    Image generatedImage = GenImageColor(width, height, BLACK);

    for (int i = 0; i < width * height; ++i) {
        if (!explored[i]) continue;
        const int type = std::countr_zero(constraints[i]);
        const int posX = i % width;
        const int posY = i / height;
        
        ImageDrawPixel(&generatedImage, posX, posY, rules.GetColor(type));
    }


    return generatedImage;

}


void Generator::DebugInit(const Ruleset& rules, int width, int height) {
    this->debugRules = rules;
    this->debugWidth = width;
    this->debugHeight = height;

    this->debugImage = GenImageColor(width, height, BLACK);
    this->debugTexture = LoadTextureFromImage(debugImage);

    this->debugExplored = std::vector<bool>(width * height, false);
    this->debugConstraints = std::vector<uint32_t>(width * height, ~((~0) << rules.GetNumberOfObjects()));
    
    int startingPoint = RandomInteger(0, width * height - 1);

    this->debugOpenSet.Push(startingPoint, startingPoint);
}


void Generator::DebugNext() {
    if (this->debugOpenSet.GetSize() <= 0) return;
    
    const int cellIndex = debugOpenSet.TopItemID();
    debugOpenSet.Pop();
    
    debugConstraints[cellIndex] = ResolveContraints(debugConstraints[cellIndex]);
    
    if (debugConstraints[cellIndex] == 0) {
        const int cellX = cellIndex % debugWidth;
        const int cellY = cellIndex / debugHeight;

        ImageDrawPixel(&debugImage, cellX, cellY, RED);

        std::printf("No possibilities on cell %i %i\n", cellX, cellY);
        return;
    }

    const int type = std::countr_zero(debugConstraints[cellIndex]);
    debugExplored[cellIndex] = true;


    // Get neighbors
    const int cellX = cellIndex % debugWidth;
    const int cellY = cellIndex / debugHeight;

    ImageDrawPixel(&debugImage, cellX, cellY, debugRules.GetColor(type));
    UpdateTexture(debugTexture, debugImage.data);

    const int boundMinY = (cellY - 1) > 0 ? (cellY - 1) : 0;
    const int boundMinX = (cellX - 1) > 0 ? (cellX - 1) : 0;
    const int boundMaxY = (cellY + 2) > debugHeight ? debugHeight : (cellY + 2);
    const int boundMaxX = (cellX + 2) > debugWidth ? debugWidth : (cellX + 2);
    
    int direction = -1;
    for (int i = boundMinY; i < boundMaxY; ++i) {
        for (int j = boundMinX; j < boundMaxX; ++j) {
            if (i == cellY && j == cellX) continue;
            direction++;
            const int nextIndex = i * debugWidth + j;
            if (debugExplored[nextIndex]) continue;
            debugConstraints[nextIndex] = AddConstraints(debugConstraints[nextIndex], debugRules.GetConstraints(type, direction));
            debugOpenSet.Push(nextIndex, nextIndex);
        }
    }

}
