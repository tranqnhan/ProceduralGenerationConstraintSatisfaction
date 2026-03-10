#include <cstdint>
#include <bit>
#include <vector>

#include "Heap.hpp"
#include "raylib.h"

#include "Ruleset.hpp"
#include "Generator.hpp"

uint32_t Generator::randomState = 12345;

Cell::Cell(const Ruleset& ruleset, const std::vector<AdjacentTile>& initialPossibilities) {
    this->tilePossibilities = std::move(initialPossibilities);
    this->globalFrequency = 0;
    for (const AdjacentTile& adj : initialPossibilities) {
        this->globalFrequency += ruleset.GetTile(adj.GetTileId()).GetGlobalFrequency();
    }
}


void Cell::Intersect(const Ruleset& ruleset, const std::vector<AdjacentTile>& possibilities) {
    std::vector<AdjacentTile> result;
    this->globalFrequency = 0;

    int a = 0;
    int b = 0;

    while (b < this->tilePossibilities.size() && a < possibilities.size()) {
        const int aTileId = possibilities[a].GetTileId();
        const int bTileId = this->tilePossibilities[b].GetTileId();

        if (aTileId < bTileId) {
            a++;
            continue;
        }

        if (aTileId > bTileId) {
            b++;
            continue;
        }

        if (aTileId == bTileId) {
            const int tileId = aTileId;
            const int localFrequency = possibilities[a].GetLocalFrequency() + this->tilePossibilities[b].GetLocalFrequency();
            result.emplace_back(tileId, localFrequency);
            a++; b++;
            this->globalFrequency += ruleset.GetTile(tileId).GetGlobalFrequency();
            continue;
        }
    }

    this->tilePossibilities = std::move(result);
}


int Cell::Solve() const {
    // Summing all frequencies
    int sumFrequencies = 0;
    for (const AdjacentTile& adj : this->tilePossibilities) {
        sumFrequencies += adj.GetLocalFrequency();
    }
    std::printf("Sum frequencies = %i\n", sumFrequencies);
    if (sumFrequencies == 0) {
        return -1;
    }

    // Get a random tile id from the possibilities
    int resultTileId;
    int rand = Generator::RandomInteger(0, sumFrequencies);
    for (const AdjacentTile& adj : this->tilePossibilities) {
        rand -= adj.GetLocalFrequency();
        if (rand <= 0) {
            resultTileId = adj.GetTileId();
            break;
        }
    }

    return resultTileId;
}
    
int Cell::GetGlobalFrequency() const {
    return this->globalFrequency;
}

Generator::Generator() {
     //std::time(0);
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
    
    // std::vector<bool> explored(width * height, false);
    // std::vector<uint32_t> constraints(width * height, ~((~0) << rules.GetNumberOfObjects()));
    
    // int startingPoint = RandomInteger(0, width * height - 1);

    // Heap<int> openSet([constraints](const int& a, const int& b) -> bool {
    //     return std::popcount(constraints[a]) > std::popcount(constraints[b]);
    // }); // stores index of next cells to solve
    
    // // TODO: since item and unique id is the same, optimize the heap to reflect this
    // openSet.Push(startingPoint, startingPoint);

    // // Constraint satisfaction
    // while (openSet.GetSize() > 0) {
    //     const int cellIndex = openSet.TopItemID();
    //     openSet.Pop();
        
    //     constraints[cellIndex] = ResolveContraints(constraints[cellIndex]);
        
    //     if (constraints[cellIndex] == 0) continue;

    //     const int type = std::countr_zero(constraints[cellIndex]);
    //     explored[cellIndex] = true;

    //     // Get neighbors
    //     const int cellX = cellIndex % width;
    //     const int cellY = cellIndex / height;

    //     const int boundMinY = (cellY - 1) > 0 ? (cellY - 1) : 0;
    //     const int boundMinX = (cellX - 1) > 0 ? (cellX - 1) : 0;
    //     const int boundMaxY = (cellY + 2) > height ? height : (cellY + 2);
    //     const int boundMaxX = (cellX + 2) > width ? width : (cellX + 2);
        
    //     int direction = -1;
    //     for (int i = boundMinY; i < boundMaxY; ++i) {
    //         for (int j = boundMinX; j < boundMaxX; ++j) {
    //             if (i == cellY && j == cellX) continue;
    //             direction++;
    //             const int nextIndex = i * width + j;
    //             if (explored[nextIndex]) continue;
    //             constraints[nextIndex] = AddConstraints(constraints[nextIndex], rules.GetConstraints(type, direction));
    //             openSet.Push(nextIndex, nextIndex);
    //         }
    //     }
    // }


    // // Generate image
    Image generatedImage = GenImageColor(width, height, BLACK);

    // for (int i = 0; i < width * height; ++i) {
    //     if (!explored[i]) continue;
    //     const int type = std::countr_zero(constraints[i]);
    //     const int posX = i % width;
    //     const int posY = i / height;
        
    //     ImageDrawPixel(&generatedImage, posX, posY, rules.GetColor(type));
    // }


    return generatedImage;

}


void Generator::DebugInit(const Ruleset& rules, int width, int height) {
    this->debugRules = rules;
    this->debugWidth = width;
    this->debugHeight = height;

    this->debugImage = GenImageColor(width, height, BLACK);
    this->debugTexture = LoadTextureFromImage(debugImage);

    this->debugExplored = std::vector<bool>(width * height, false);
    
    this->debugCellsIndex = std::vector<int>(width * height, -1);
    this->debugCells = std::vector<Cell>();

    int initialCoords = RandomInteger(0, width * height - 1);

    std::vector<AdjacentTile> initialTiles;
    for (int i = 0; i < rules.GetNumTiles(); ++i) {
        const Tile& tile = rules.GetTile(i);
        initialTiles.emplace_back(i, tile.GetGlobalFrequency());
    }

    this->debugCells.emplace_back(rules, initialTiles);
    this->debugCellsIndex[initialCoords] = this->debugCells.size() - 1;

    this->debugOpenSet.Push(initialCoords, initialCoords);
}


void Generator::DebugPropagation(const Tile& tile, int coords) {
    const int x = coords % this->debugWidth;
    const int y = coords / this->debugWidth;
    
    if (x + 1 < this->debugWidth) {
        DebugExpandAdjacent(x + 1, y, TileDirection::EAST, tile);
    }

    if (x - 1 >= 0) {
        DebugExpandAdjacent(x - 1, y, TileDirection::WEST, tile);
    }

    if (y + 1 < this->debugHeight) {
        DebugExpandAdjacent(x, y + 1, TileDirection::SOUTH, tile);
    }

    if (y - 1 >= 0) {
        DebugExpandAdjacent(x, y - 1, TileDirection::NORTH, tile);
    }
}


void Generator::DebugExpandAdjacent(int x, int y, TileDirection direction, const Tile& tile) {
    const int adjCoords = y * this->debugWidth + x;
    
    if (this->debugExplored[adjCoords]) return;

    const std::vector<AdjacentTile>& adjTiles = tile.GetAdjacentTiles(direction);
    if (this->debugCellsIndex[adjCoords] == -1) {
        this->debugCells.emplace_back(this->debugRules, adjTiles);
        this->debugCellsIndex[adjCoords] = this->debugCells.size() - 1;
    } else {
        this->debugCells[this->debugCellsIndex[adjCoords]].Intersect(this->debugRules, adjTiles);
    }
    this->debugOpenSet.Push(adjCoords, adjCoords);
}


void Generator::DebugNext() {
    std::printf("Debug next! %i\n", this->debugOpenSet.GetSize());
    if (this->debugOpenSet.GetSize() <= 0) return;

    const int coords = debugOpenSet.TopItemID();
    debugOpenSet.Pop();

    const int solvedTileId = this->debugCells[this->debugCellsIndex[coords]].Solve();
    std::printf("Solved Tile! %i\n", solvedTileId);

    if (solvedTileId == -1) {
        const int cellX = coords % debugWidth;
        const int cellY = coords / debugHeight;

        ImageDrawPixel(&debugImage, cellX, cellY, RED);
        UpdateTexture(debugTexture, debugImage.data);
        //std::printf("No possibilities on cell %i %i\n", cellX, cellY);
        return;
    }

    this->debugExplored[coords] = true;

    const int cellX = coords % debugWidth;
    const int cellY = coords / debugHeight;

    const Tile& tile = this->debugRules.GetTile(solvedTileId);
    const uint32_t compressedColor = tile.GetColor();
    const Color color = Color{
        .r = (unsigned char)((compressedColor & 0xFF0000) >> 16), 
        .g = (unsigned char)((compressedColor & 0xFF00) >> 8),
        .b = (unsigned char)((compressedColor & 0xFF)),
        .a = 255
    };

    std::printf("color! %b\n", compressedColor);

    ImageDrawPixel(&debugImage, cellX, cellY, color);
    UpdateTexture(debugTexture, debugImage.data);

    // Add adjacents
    DebugPropagation(tile, coords);
}
