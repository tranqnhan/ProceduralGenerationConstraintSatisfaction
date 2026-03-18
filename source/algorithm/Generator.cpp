#include <cstdint>
#include <cstdio>
#include <vector>

#include <raylib.h>

#include "BitMath.hpp"
#include "Heap.hpp"
#include "Program.hpp"
#include "XorshiftRandom.hpp"
#include "CompressColor.hpp"
#include "Ruleset.hpp"
#include "Generator.hpp"


Cell::Cell(const Ruleset& ruleset) {
    this->tilePossibilities.resize(ruleset.GetTile64Sets(), ~uint64_t(0));

    uint64_t& tileSet = this->tilePossibilities.back();
    tileSet <<= 64 - (ruleset.GetNumberOfTiles() % 64);

    this->globalFrequency = 0;
    this->resultTileId = SpecialCellType::Unexplored;
    this->numberOfPossibleTiles = ruleset.GetNumberOfTiles();
}


bool Cell::Intersect(const std::vector<uint64_t>& otherPossibilities) {
    int changedIndex = -1;
    
    for (int i = 0; i < otherPossibilities.size(); ++i) {
        const uint64_t mask = this->tilePossibilities[i] & otherPossibilities[i];
        if (this->tilePossibilities[i] != mask) {
            this->tilePossibilities[i] = mask;
            changedIndex = i + 1;
            break;
        }
    }

    if (changedIndex >= 0) {
        for (int i = changedIndex; i < otherPossibilities.size(); ++i) {
            this->tilePossibilities[i] &= otherPossibilities[i];
        }

        this->numberOfPossibleTiles = 0;
        for (const uint64_t& tileSet : this->tilePossibilities) {
            this->numberOfPossibleTiles += std::popcount(tileSet);
        }
    }

    return changedIndex >= 0;
}


int Cell::Collapse(const Ruleset& ruleset) {
    if (this->resultTileId != SpecialCellType::Unexplored) return this->resultTileId;

    this->resultTileId = SpecialCellType::NoSolution;

    if (this->numberOfPossibleTiles == 0) return this->resultTileId;

    const std::vector<int> tileIds = BitMath::GetSetPositions(this->tilePossibilities);

    std::vector<int> globalFrequencies(tileIds.size());

    int sumGlobalFrequencies = 0;
    for (int i = 0; i < tileIds.size(); ++i) {
        sumGlobalFrequencies += ruleset.GetTileFrequency(tileIds[i]);
        globalFrequencies[i] = sumGlobalFrequencies;
    }

    int result = XorshiftRandom::RandomInteger(0, sumGlobalFrequencies);
    
    for (int i = 0; i < globalFrequencies.size(); ++i) {
        if (result <= globalFrequencies[i]) {
            this->resultTileId = tileIds[i];
            break;
        }
    }

    return this->resultTileId;
}


const std::vector<uint64_t>& Cell::GetTilePossibilities() const {
    return this->tilePossibilities;
}

    
int Cell::GetEntropy() const {
    return this->numberOfPossibleTiles;
}


int Cell::GetResultTile() const {
    return this->resultTileId;
}


void Cell::Clear(const Ruleset& ruleset) {
    this->tilePossibilities.resize(ruleset.GetTile64Sets(), ~uint64_t(0));

    uint64_t& tileSet = this->tilePossibilities.back();
    tileSet <<= 64 - (ruleset.GetNumberOfTiles() % 64);

    this->globalFrequency = 0;
    this->resultTileId = SpecialCellType::Unexplored;
    this->numberOfPossibleTiles = ruleset.GetNumberOfTiles();
}


Generator::Generator() {}


void Generator::Init(const Ruleset& rules, int regionWidthAsPixels, int regionHeightAsPixels, int worldWidthAsRegions, int worldHeightAsRegions) {
    this->ruleset = rules;
    this->worldWidthAsPixels = regionWidthAsPixels * worldWidthAsRegions;
    this->worldHeightAsPixels = regionHeightAsPixels * worldHeightAsRegions;

    this->regionWidthAsPixels = regionWidthAsPixels;
    this->regionHeightAsPixels = regionHeightAsPixels;
    this->worldWidthAsRegions = worldWidthAsRegions;
    this->worldHeightAsRegions = worldHeightAsRegions;

    this->generatedImage = GenImageColor(this->worldWidthAsPixels, this->worldHeightAsPixels, BLACK);
    this->generatedTexture = LoadTextureFromImage(this->generatedImage);

    this->xRegionOfWorld = 0;
    this->yRegionOfWorld = 0;
    this->numberOfReset = 0;

    const Cell initialCell( this->ruleset);
    this->cells = std::vector<Cell>(this->worldWidthAsPixels * this->worldHeightAsPixels, initialCell);
    this->isRegionsGenerated = std::vector<bool>(this->worldWidthAsRegions * this->worldHeightAsRegions, false);

    this->BuildInitialRegion();
}


void Generator::BuildInitialRegion() {
    const int pixelCoordsOfRegion = XorshiftRandom::RandomInteger(0, this->regionWidthAsPixels * this->regionHeightAsPixels - 1);
    
    const int xPixelOfRegion = pixelCoordsOfRegion % this->regionWidthAsPixels;
    const int yPixelOfRegion = pixelCoordsOfRegion / this->regionHeightAsPixels;
    
    const int xPixelOfWorld = this->xRegionOfWorld * this->regionWidthAsPixels + xPixelOfRegion;
    const int yPixelOfWorld = this->yRegionOfWorld * this->regionHeightAsPixels + yPixelOfRegion;

    this->cellEntropyPriorityQueue.Push(0, yPixelOfWorld * this->worldWidthAsPixels + xPixelOfWorld);
}


void Generator::BuildCurrentRegion() {
    this->cellEntropyPriorityQueue.Clear();

    const int regionCoordsOfWorld = this->yRegionOfWorld * this->worldWidthAsRegions + this->xRegionOfWorld;
    const int xPixelOfWorld = this->xRegionOfWorld * this->regionWidthAsPixels;
    const int yPixelOfWorld = this->yRegionOfWorld * this->regionHeightAsPixels;
    printf("Building next region! %i %i \n", this->xRegionOfWorld, this->yRegionOfWorld);

    if (this->xRegionOfWorld - 1 >= 0 && this->isRegionsGenerated[regionCoordsOfWorld - 1]) {
        for (int y = yPixelOfWorld; y < (yPixelOfWorld + this->regionHeightAsPixels); ++y) {
            const int coordsPixelOfWorld = y * this->worldWidthAsPixels + (xPixelOfWorld - 1);
            this->cellEntropyPriorityQueue.Push(0, coordsPixelOfWorld);
        }
    }

    if (this->xRegionOfWorld + 1 < this->worldWidthAsRegions && this->isRegionsGenerated[regionCoordsOfWorld + 1]) {
        for (int y = yPixelOfWorld; y < (yPixelOfWorld + this->regionHeightAsPixels); ++y) {
            const int coordsPixelOfWorld = y * this->worldWidthAsPixels + (xPixelOfWorld + 1);
            this->cellEntropyPriorityQueue.Push(0, coordsPixelOfWorld);
        }
    }

    if (this->yRegionOfWorld - 1 >= 0 && this->isRegionsGenerated[regionCoordsOfWorld - this->worldWidthAsRegions]) {
        for (int x = xPixelOfWorld; x < (xPixelOfWorld + this->regionWidthAsPixels); ++x) {
            const int coordsPixelOfWorld = (yPixelOfWorld - 1) * this->worldWidthAsPixels + x;
            this->cellEntropyPriorityQueue.Push(0, coordsPixelOfWorld);
        }
    }

    if (this->yRegionOfWorld + 1 >= 0 && this->isRegionsGenerated[regionCoordsOfWorld + this->worldWidthAsRegions]) {
        for (int x = xPixelOfWorld; x < (xPixelOfWorld + this->regionWidthAsPixels); ++x) {
            const int coordsPixelOfWorld = (yPixelOfWorld + 1) * this->worldWidthAsPixels + x;
            this->cellEntropyPriorityQueue.Push(0, coordsPixelOfWorld);
        }
    }
}


// States
// Region Generation Failure
// Region Currently Generating
// Region Completed
// World Completed


int Generator::GetNextRegion() {

}


void Generator::Next() {
    switch(this->generationState) {
        case RegionFailure:
            // handle failure
            break;
        case RegionInProgress:
            //handle in progress
            break;
        case RegionSuccess:
            // handle region success
            break;
        case WorldSuccess:
            return;
    }


    if (this->regionGenerationFailure) {    
        this->ResetRegion(, int yRegion)
        return;
    }

    if (this->cellEntropyPriorityQueue.GetSize() <= 0) {
        const int regionCoordsOfWorld = this->yRegionOfWorld * this->worldWidthAsRegions + this->xRegionOfWorld;
        this->isRegionsGenerated[regionCoordsOfWorld] = true;
        this->regionsGenerated.emplace_back(regionCoordsOfWorld);

        if (this->xRegionOfWorld < this->worldWidthAsRegions - 1) {
            this->xRegionOfWorld++;
            this->BuildCurrentRegion();
        } else if (this->yRegionOfWorld < this->worldHeightAsRegions - 1) { 
            this->xRegionOfWorld = 0;
            this->yRegionOfWorld++;
            this->BuildCurrentRegion();
        } else {
            return;
        }
    }

    const int currentCoordinates = this->cellEntropyPriorityQueue.TopItemID();
    this->cellEntropyPriorityQueue.Pop();

    const int resultTileId = this->cells[currentCoordinates].Collapse(this->ruleset);

    const int cellX = currentCoordinates % worldWidthAsPixels;
    const int cellY = currentCoordinates / worldWidthAsPixels;

    const Tile& tile = this->ruleset.GetTile(resultTileId);
    const uint32_t compressedColor = tile.GetColor()[0];

    ImageDrawPixel(&generatedImage, cellX, cellY, CompressColor::Decompress(compressedColor));
    UpdateTexture(generatedTexture, generatedImage.data);

    // Propagation
    CompletePropagation(currentCoordinates);

    if (this->cellEntropyPriorityQueue.GetSize() <= 0 && 
        this->generationState != GenerationState::RegionFailure) {
        this->generationState = GenerationState::RegionSuccess;
    } else {
        this->generationState = GenerationState::RegionInProgress;
    }
}


void Generator::CompletePropagation(int beginCoordinates) {
    std::vector<int> queueCoordinates;
    std::vector<bool> isInQueue(this->worldWidthAsPixels * this->worldHeightAsPixels);

    queueCoordinates.emplace_back(beginCoordinates);
    
    while (!queueCoordinates.empty() && this->generationState != GenerationState::RegionFailure) {
        const int currentCoordinates = queueCoordinates.back();
        queueCoordinates.pop_back();

        isInQueue[currentCoordinates] = false;

        const Cell& currentCell = this->cells[currentCoordinates];

        Propagate(currentCoordinates, queueCoordinates, isInQueue);
    }
}


void Generator::Propagate(int coordinates, 
    std::vector<int>& queueCoordinates,
    std::vector<bool>& isInQueue
) {
    const Cell& cell = this->cells[coordinates];

    const int x = coordinates % this->worldWidthAsPixels;
    const int y = coordinates / this->worldWidthAsPixels;

    const int xPixelMinBound = this->xRegionOfWorld * this->regionWidthAsPixels;
    const int xPixelMaxBound = xPixelMinBound + this->regionWidthAsPixels;
    const int yPixelMinBound = this->yRegionOfWorld * this->regionHeightAsPixels;
    const int yPixelMaxBound = yPixelMinBound + this->regionHeightAsPixels;

    if (x + 1 < xPixelMaxBound) {
        const int adjacentCoordinates = coordinates + 1;
        this->ExpandAdjacent(adjacentCoordinates, TileDirection::EAST, cell, queueCoordinates, isInQueue);
    }

    if (x - 1 >= xPixelMinBound) {
        const int adjacentCoordinates = coordinates - 1;
        this->ExpandAdjacent(adjacentCoordinates, TileDirection::WEST, cell, queueCoordinates, isInQueue);
    }

    if (y + 1 < yPixelMaxBound) {
        const int adjacentCoordinates = coordinates + this->worldWidthAsPixels;
        this->ExpandAdjacent(adjacentCoordinates, TileDirection::SOUTH, cell, queueCoordinates, isInQueue);
    }

    if (y - 1 >= yPixelMinBound) {
        const int adjacentCoordinates = coordinates - this->worldWidthAsPixels;
        this->ExpandAdjacent(adjacentCoordinates, TileDirection::NORTH, cell, queueCoordinates, isInQueue);
    }
}


void Generator::ExpandAdjacent(int adjacentCoordinates, 
    TileDirection direction, 
    const Cell& cell, 
    std::vector<int>& queueCoordinates,
    std::vector<bool>& isInQueue
) {

    Cell& adjacentCell = this->cells[adjacentCoordinates];

    if (adjacentCell.GetResultTile() >= SpecialCellType::NoSolution) return;

    const int cellSolvedTiled = cell.GetResultTile();

    bool changes;

    if (cellSolvedTiled < 0) {
        std::vector<uint64_t> adjacentTilesUnion(this->ruleset.GetTile64Sets(), 0);

        const std::vector<int> tileIds = BitMath::GetSetPositions(cell.GetTilePossibilities());
        
        for (const int tileId : tileIds) {
            const Tile& tile = this->ruleset.GetTile(tileId);
            const std::vector<uint64_t>& adjacentTiles = tile.GetAdjacentTiles(direction);

            for (int k = 0; k < adjacentTiles.size(); ++k) {
                adjacentTilesUnion[k] |= adjacentTiles[k];
            }
        }

        changes = adjacentCell.Intersect(adjacentTilesUnion);
        
    } else {

        const Tile& tile = ruleset.GetTile(cellSolvedTiled);
        changes = adjacentCell.Intersect(tile.GetAdjacentTiles(direction));
    
    }

    if (changes) {
        if (!isInQueue[adjacentCoordinates]) {
            queueCoordinates.emplace_back(adjacentCoordinates);
            isInQueue[adjacentCoordinates] = true;
        }
    }

    const int entropy = adjacentCell.GetEntropy();
    
    if (entropy == 0) {
        this->generationState = GenerationState::RegionFailure;
        return;
    }
    
    this->cellEntropyPriorityQueue.Push(entropy, adjacentCoordinates);
}


void Generator::ResetRegion(int xRegionOfWorld, int yRegionOfWorld) {
    const int beginY = yRegionOfWorld * this->regionHeightAsPixels;
    const int beginX = xRegionOfWorld * this->regionWidthAsPixels;

    for (int y = beginY; y < beginY + this->regionHeightAsPixels; ++y) {
        for (int x = beginX; x < beginX + this->regionWidthAsPixels; ++x) {
            this->cells[y * this->worldWidthAsPixels + x].Clear(this->ruleset);
        }
    }
}


int Generator::GetCellTileId(int coordinates) const {
    if (coordinates >= this->cells.size()) return SpecialCellType::Unexplored;
    return this->cells[coordinates].GetResultTile();
}


const Cell& Generator::GetCell(int coordinates) const {
    return this->cells[coordinates];
}


void Generator::Render() {
    DrawTextureEx(this->generatedTexture, Vector2{.x = 0, .y = 0}, 0, PIXEL_SCALE, WHITE);
}


const Ruleset& Generator::GetRuleset() const {
    return this->ruleset;
}


int Generator::GetWidth() const {
    return this->worldWidthAsPixels;
}


int Generator::GetHeight() const {
    return this->worldHeightAsPixels;
}


Generator::~Generator() {
    UnloadImage(this->generatedImage);
    UnloadTexture(this->generatedTexture);
}