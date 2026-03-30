#include <cstdint>
#include <vector>

#include <raylib.h>

#include "Heap.hpp"
#include "Program.hpp"
#include "XorshiftRandom.hpp"
#include "CompressColor.hpp"
#include "Ruleset.hpp"
#include "Generator.hpp"

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

    this->xRegionOfWorld = -1;
    this->yRegionOfWorld = 0;
    this->currentRegionId = -1;

    const Cell initialCell( this->ruleset);
    this->cells = std::vector<Cell>(this->worldWidthAsPixels * this->worldHeightAsPixels, initialCell);
    this->isRegionsGenerated = std::vector<bool>(this->worldWidthAsRegions * this->worldHeightAsRegions, false);

    this->cellEntropyPriorityQueue.Clear();

    this->generationState = GenerationState::RegionOnStandby;
}


void Generator::BuildInitialRegion() {
    this->cellEntropyPriorityQueue.Clear();

    const int pixelCoordsOfRegion = XorshiftRandom::RandomInteger(0, this->regionWidthAsPixels * this->regionHeightAsPixels - 1);
    
    const int xPixelOfRegion = pixelCoordsOfRegion % this->regionWidthAsPixels;
    const int yPixelOfRegion = pixelCoordsOfRegion / this->regionWidthAsPixels;
    
    const int xPixelOfWorld = this->xRegionOfWorld * this->regionWidthAsPixels + xPixelOfRegion;
    const int yPixelOfWorld = this->yRegionOfWorld * this->regionHeightAsPixels + yPixelOfRegion;

    this->cellEntropyPriorityQueue.Push(0, yPixelOfWorld * this->worldWidthAsPixels + xPixelOfWorld);
}


void Generator::BuildCurrentRegion() {
    this->cellEntropyPriorityQueue.Clear();

    const int regionCoordsOfWorld = this->yRegionOfWorld * this->worldWidthAsRegions + this->xRegionOfWorld;
    const int xPixelOfWorld = this->xRegionOfWorld * this->regionWidthAsPixels;
    const int yPixelOfWorld = this->yRegionOfWorld * this->regionHeightAsPixels;

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

    if (this->yRegionOfWorld + 1 < this->worldHeightAsRegions && this->isRegionsGenerated[regionCoordsOfWorld + this->worldWidthAsRegions]) {
        for (int x = xPixelOfWorld; x < (xPixelOfWorld + this->regionWidthAsPixels); ++x) {
            const int coordsPixelOfWorld = (yPixelOfWorld + 1) * this->worldWidthAsPixels + x;
            this->cellEntropyPriorityQueue.Push(0, coordsPixelOfWorld);
        }
    }
}

void Generator::GenerateNextCell() {
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
    Propagate(currentCoordinates);

    if (this->cellEntropyPriorityQueue.GetSize() <= 0) {
        const int regionCoordsOfWorld = this->yRegionOfWorld * this->worldWidthAsRegions + this->xRegionOfWorld;
        this->isRegionsGenerated[regionCoordsOfWorld] = true;
        this->regionsFailures[this->currentRegionId] = 0;
    }
}


void Generator::StartNextRegion() {
    // Completed a region 
    this->currentRegionId++;

    if (this->currentRegionId == this->regionsCoords.size()) {
        if (this->xRegionOfWorld == this->worldWidthAsRegions - 1) {
            this->xRegionOfWorld = 0;
            this->yRegionOfWorld++;
        } else {
            this->xRegionOfWorld++;
        }

        const int coords = this->yRegionOfWorld * this->worldWidthAsRegions + this->xRegionOfWorld;
        this->regionsCoords.emplace_back(coords);
        this->regionsFailures.emplace_back(0);
    } else {
        const int coords = this->regionsCoords[this->currentRegionId];
        this->xRegionOfWorld = coords % this->worldWidthAsRegions;
        this->yRegionOfWorld = coords / this->worldWidthAsRegions;
    }

    if (currentRegionId == 0) {
        this->BuildInitialRegion();
    } else {
        this->BuildCurrentRegion();
    }
}


void Generator::Next() {
    switch(this->generationState) {
        case RegionFailure:
            this->BacktrackRegions();
            this->generationState = GenerationState::RegionOnStandby;
            break;

        case RegionInProgress:
            this->GenerateNextCell();
            
            if (this->generationState != GenerationState::RegionFailure) {
                if (this->cellEntropyPriorityQueue.GetSize() <= 0) {
                    
                if (this->xRegionOfWorld >= this->worldWidthAsRegions - 1 &&
                    this->yRegionOfWorld >= this->worldHeightAsRegions - 1) {
                    this->generationState = GenerationState::WorldSuccess;
                } else {
                    this->generationState = GenerationState::RegionOnStandby;
                }
                } else {
                    this->generationState = GenerationState::RegionInProgress;
                }
            } else {
                this->cellEntropyPriorityQueue.Clear();
            }
            
            break;

        case RegionOnStandby:
            this->StartNextRegion();
            this->generationState = GenerationState::RegionInProgress;
            break;

        case WorldSuccess:
            return;
    }
}


void Generator::Propagate(int beginCoordinates) {
    std::vector<int> queueCoordinates;
    std::vector<bool> isInQueue(this->worldWidthAsPixels * this->worldHeightAsPixels);

    queueCoordinates.emplace_back(beginCoordinates);
    
    while (!queueCoordinates.empty() && this->generationState != GenerationState::RegionFailure) {
        const int currentCoordinates = queueCoordinates.back();
        queueCoordinates.pop_back();

        isInQueue[currentCoordinates] = false;

        const Cell& currentCell = this->cells[currentCoordinates];

        this->Expand(currentCoordinates, queueCoordinates, isInQueue);
    }
}


void Generator::Expand(
    int coordinates, 
    std::vector<int>& queueCoordinates,
    std::vector<bool>& isInQueue
) {
    Cell& cell = this->cells[coordinates];

    // Todo: possible optimization here
    const int x = coordinates % this->worldWidthAsPixels;
    const int y = coordinates / this->worldWidthAsPixels;

    const int xPixelMinBound = this->xRegionOfWorld * this->regionWidthAsPixels;
    const int xPixelMaxBound = xPixelMinBound + this->regionWidthAsPixels;
    const int yPixelMinBound = this->yRegionOfWorld * this->regionHeightAsPixels;
    const int yPixelMaxBound = yPixelMinBound + this->regionHeightAsPixels;

    const std::vector<int>& tilePossibilitiesIds = cell.GetTilePossibilitiesAsIds();

    if (x + 1 < xPixelMaxBound) {
        const int adjacentCoordinates = coordinates + 1;
        this->ContraintAdjacent(adjacentCoordinates, TileDirection::EAST, tilePossibilitiesIds, queueCoordinates, isInQueue);
    }

    if (x - 1 >= xPixelMinBound) {
        const int adjacentCoordinates = coordinates - 1;
        this->ContraintAdjacent(adjacentCoordinates, TileDirection::WEST, tilePossibilitiesIds, queueCoordinates, isInQueue);
    }

    if (y + 1 < yPixelMaxBound) {
        const int adjacentCoordinates = coordinates + this->worldWidthAsPixels;
        this->ContraintAdjacent(adjacentCoordinates, TileDirection::SOUTH, tilePossibilitiesIds, queueCoordinates, isInQueue);
    }

    if (y - 1 >= yPixelMinBound) {
        const int adjacentCoordinates = coordinates - this->worldWidthAsPixels;
        this->ContraintAdjacent(adjacentCoordinates, TileDirection::NORTH, tilePossibilitiesIds, queueCoordinates, isInQueue);
    }
}


void Generator::ContraintAdjacent(
    int adjacentCoordinates, 
    TileDirection direction,
    const std::vector<int>& tilePossibilitiesIds,
    std::vector<int>& queueCoordinates,
    std::vector<bool>& isInQueue
) {

    Cell& adjacentCell = this->cells[adjacentCoordinates];

    if (adjacentCell.GetResultTile() >= SpecialCellType::NoSolution) return;

    bool changes;

    std::vector<uint64_t> adjacentTilesUnion(this->ruleset.GetTile64Sets(), 0);

    for (const int tileId : tilePossibilitiesIds) {
        const Tile& tile = this->ruleset.GetTile(tileId);
        const std::vector<uint64_t>& adjacentTiles = tile.GetAdjacentTiles(direction);

        for (int k = 0; k < adjacentTiles.size(); ++k) {
            adjacentTilesUnion[k] |= adjacentTiles[k];
        }
    }

    changes = adjacentCell.Intersect(adjacentTilesUnion);
    
    
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

void Generator::BacktrackRegions() {
    this->regionsFailures[this->currentRegionId]++;

    this->ResetRegion(this->xRegionOfWorld, this->yRegionOfWorld);

    const int numRegionsRollback = this->regionsFailures[this->currentRegionId];
    int beginRollbackIndex = this->currentRegionId - numRegionsRollback;
    
    if (beginRollbackIndex < 0) {
        beginRollbackIndex = 0;
    }

    for (int i = beginRollbackIndex; i < this->currentRegionId; ++i) {
        const int xRegion = this->regionsCoords[i] % this->worldWidthAsRegions;
        const int yRegion = this->regionsCoords[i] / this->worldWidthAsRegions;
        this->ResetRegion(xRegion, yRegion);
    }
    UpdateTexture(generatedTexture, generatedImage.data);

    this->currentRegionId = beginRollbackIndex - 1;
}


void Generator::ResetRegion(int xRegionOfWorld, int yRegionOfWorld) {
    const int beginX = xRegionOfWorld * this->regionWidthAsPixels;
    const int beginY = yRegionOfWorld * this->regionHeightAsPixels;

    for (int y = beginY; y < beginY + this->regionHeightAsPixels; ++y) {
        for (int x = beginX; x < beginX + this->regionWidthAsPixels; ++x) {
            this->cells[y * this->worldWidthAsPixels + x].Clear(this->ruleset);

            ImageDrawPixel(&generatedImage, x, y, BLACK);
        }
    }

    this->isRegionsGenerated[yRegionOfWorld * this->worldWidthAsRegions + xRegionOfWorld] = false;
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