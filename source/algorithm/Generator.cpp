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
                if (i == 0 && j == 0) continue;
                direction++;
                const int nextIndex = i * width + j;
                if (explored[nextIndex]) continue;
                constraints[nextIndex] = AddConstraints(constraints[nextIndex], rules.GetConstraints(type, direction));
                openSet.Push(nextIndex, nextIndex);
            }
        }
    }


    // Generate image
    Image generateImage = GenImageColor(width, height, BLACK);

    for (int i = 0; i < width * height; ++i) {
        if (!explored[i]) continue;
        const int type = std::countr_zero(constraints[i]);
        const int posX = i % width;
        const int posY = i / height;
        
        ImageDrawPixel(&generateImage, posX, posY, rules.GetColor(type));
    }


    return generateImage;

}