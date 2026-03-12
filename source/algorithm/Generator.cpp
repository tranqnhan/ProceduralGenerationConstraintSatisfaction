#include <algorithm>
#include <cstdint>
#include <bit>
#include <queue>
#include <utility>
#include <vector>

#include "Heap.hpp"
#include "XorshiftRandom.hpp"
#include "raylib.h"

#include "Ruleset.hpp"
#include "Generator.hpp"


Cell::Cell(const Ruleset& ruleset) {
    this->tilePossibilities.resize(int(ruleset.GetNumberOfTiles() / 64), ~uint64_t(0));
    this->globalFrequency = 0;
    this->solvedTileId = -1;
}


bool Cell::Intersect(const std::vector<uint64_t>& otherPossibilities) {
    bool changes = false;
    
    for (int i = 0; i < otherPossibilities.size(); ++i) {
        if (this->tilePossibilities[i] != otherPossibilities[i]) {
            changes = true;
            break;
        }
    }

    if (changes) {
        for (int i = 0; i < otherPossibilities.size(); ++i) {
            this->tilePossibilities[i] &= otherPossibilities[i];
        }
    }

    return changes;
}


int Cell::Collapse() {
    std::vector<int> tilesCount(this->tilePossibilities.size());
    int sumTiles = 0;
    for (int i = 0; i < this->tilePossibilities.size(); ++i) {
        const int tileCount = std::popcount(this->tilePossibilities[i]);
        tilesCount[i] = tileCount;
        sumTiles += tileCount;
    }

    int result = XorshiftRandom::RandomInteger(1, sumTiles);
    
    for (int i = 0; i < tilesCount.size(); ++i) {
        if (result > tilesCount[i]) {
            result -= tilesCount[i];
        } else {
            int tileCountInSet = 0;
            uint64_t tileSet = this->tilePossibilities[i];
            const uint64_t mask = uint64_t(1) << 63;
            while (result > 0) {
                if (mask & tileSet) {
                    result--;
                }
                tileSet <<= 1;
                tileCountInSet++;
            }
            tileCountInSet--;
            this->solvedTileId = i * 64 + tileCountInSet;
            break;
        }
    }


    return this->solvedTileId;
}


const std::vector<uint64_t>& Cell::GetTilePossibilities() const {
    return this->tilePossibilities;
}

    
int Cell::GetEntropy() const {

}


int Cell::GetSolvedTile() const {
    return this->solvedTileId;
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

    int pick = XorshiftRandom::RandomInteger(1, totalPossibilities);

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


void Generator::Init(const Ruleset& rules, int width, int height) {
    this->ruleset = rules;
    this->width = width;
    this->height = height;

    this->debugImage = GenImageColor(width, height, BLACK);
    this->debugTexture = LoadTextureFromImage(debugImage);

    const Cell initialCell( this->ruleset);

    this->cells = std::vector<Cell>(width * height, initialCell);

    int initialCoords = XorshiftRandom::RandomInteger(0, width * height - 1);

    this->cellEntropyPriorityQueue.Push(0, initialCoords);
}

void Generator::Next() {
    if (this->cellEntropyPriorityQueue.GetSize() <= 0) return;

    const int currentCoordinates = this->cellEntropyPriorityQueue.TopItemID();
    this->cellEntropyPriorityQueue.Pop();

  //  std::printf("coords %i cell index %i\n", coords, cellIndex);
    const int solvedTileId = this->cells[currentCoordinates].Collapse();
    // std::printf("Solved Tile! %i\n", solvedTileId);

    if (solvedTileId == -1) {
        const int cellX = currentCoordinates % width;
        const int cellY = currentCoordinates / height;

        ImageDrawPixel(&debugImage, cellX, cellY, RED);
        UpdateTexture(debugTexture, debugImage.data);
        //std::printf("No possibilities on cell %i %i\n", cellX, cellY);
        return;
    }

    const int cellX = currentCoordinates % width;
    const int cellY = currentCoordinates / height;

    const Tile& tile = this->ruleset.GetTile(solvedTileId);
    const uint32_t compressedColor = tile.GetColor();
    const Color color = Color{
        .r = (unsigned char)((compressedColor & 0xFF0000) >> 16), 
        .g = (unsigned char)((compressedColor & 0xFF00) >> 8),
        .b = (unsigned char)((compressedColor & 0xFF)),
        .a = 255
    };

    ImageDrawPixel(&debugImage, cellX, cellY, color);
    UpdateTexture(debugTexture, debugImage.data);



    // Propagation
    CompletePropagation(currentCoordinates);
}


void Generator::CompletePropagation(int beginCoordinates) {
    std::queue<int> queueCoordinates;
    ankerl::unordered_dense::set<int> propagatedCoordinates;
    ankerl::unordered_dense::set<int> exploredCoordinates;

    queueCoordinates.push(beginCoordinates);
    
    while (!queueCoordinates.empty()) {
        const int currentCoordinates = queueCoordinates.front();
        queueCoordinates.pop();

        //std::printf("current Coords = %i\n", currentCoordinates);
        exploredCoordinates.emplace(currentCoordinates);

        const Cell& currentCell = this->cells[currentCoordinates];

        Propagate(currentCoordinates, queueCoordinates, propagatedCoordinates, exploredCoordinates);        
    }
}

void Generator::Propagate(int coordinates, 
    std::queue<int>& queueCoordinates, 
    ankerl::unordered_dense::set<int>& propagatedCoordinates, 
    const ankerl::unordered_dense::set<int>& exploredCoordinates
) {
    const Cell& cell = this->cells[coordinates];

    const int x = coordinates % this->width;
    const int y = coordinates / this->width;
    
    if (x + 1 < this->width) {
        const int adjacentCoordinates = coordinates + 1;
        this->ExpandAdjacent(adjacentCoordinates, TileDirection::EAST, cell, queueCoordinates, propagatedCoordinates, exploredCoordinates);
    }

    if (x - 1 >= 0) {
        const int adjacentCoordinates = coordinates - 1;
        this->ExpandAdjacent(adjacentCoordinates, TileDirection::WEST, cell, queueCoordinates, propagatedCoordinates, exploredCoordinates);
    }

    if (y + 1 < this->height) {
        const int adjacentCoordinates = coordinates + this->width;
        this->ExpandAdjacent(adjacentCoordinates, TileDirection::SOUTH, cell, queueCoordinates, propagatedCoordinates, exploredCoordinates);
    }

    if (y - 1 >= 0) {
        const int adjacentCoordinates = coordinates - this->width;
        this->ExpandAdjacent(adjacentCoordinates, TileDirection::NORTH, cell, queueCoordinates, propagatedCoordinates, exploredCoordinates);
    }
}


void Generator::ExpandAdjacent(int adjacentCoordinates, 
    TileDirection direction, 
    const Cell& cell, 
    std::queue<int>& queueCoordinates, 
    ankerl::unordered_dense::set<int>& propagatedCoordinates,
    const ankerl::unordered_dense::set<int>& exploredCoordinates
) {
    if (exploredCoordinates.find(adjacentCoordinates) != exploredCoordinates.end()) return;

    Cell& adjacentCell = this->cells[adjacentCoordinates];

    if (adjacentCell.GetSolvedTile() >= 0) return;

    const int cellSolvedTiled = cell.GetSolvedTile();

    bool changes;

    if (cellSolvedTiled < 0) {
    
        std::vector<uint64_t> adjacentTilesUnion(int(this->ruleset.GetNumberOfTiles() / 64), 0);

        constexpr uint64_t mask = uint64_t(1) << 63;
        const std::vector<uint64_t>& tilePossibilities = cell.GetTilePossibilities();
        for (int i = 0; i < tilePossibilities.size(); i++) {
            if (tilePossibilities[i] == 0) continue;
            uint32_t tileSet = tilePossibilities[i];
            for (int j = 0; j < 64; ++j) {
                if (!(mask & tileSet)) continue;
                
                const int tileId = i * 64 + j;
                const Tile& tile = this->ruleset.GetTile(tileId);

                const std::vector<uint64_t>& adjacentTiles = tile.GetAdjacentTiles(direction);

                for (int k = 0; k < adjacentTiles.size(); ++k) {
                    adjacentTilesUnion[i] |= adjacentTiles[i];
                }

                tileSet <<= 1;
            }
        }


        changes = adjacentCell.Intersect(adjacentTilesUnion);
    
    } else {
    
        const Tile& tile = ruleset.GetTile(cellSolvedTiled);
        changes = adjacentCell.Intersect(tile.GetAdjacentTiles(direction));
    
    }

    if (changes) {
        const int entropy = adjacentCell.GetEntropy();
        this->cellEntropyPriorityQueue.Push(entropy, adjacentCoordinates);

        if (propagatedCoordinates.find(adjacentCoordinates) == propagatedCoordinates.end()) {
            queueCoordinates.push(adjacentCoordinates);
            propagatedCoordinates.emplace(adjacentCoordinates);
        }
    }
}