#pragma once

#include <vector>

#include <raylib.h>

#include "Heap.hpp"
#include "Ruleset.hpp"
#include "Cell.hpp"

enum GenerationState {
    RegionOnStandby,
    RegionFailure,
    RegionInProgress,
    WorldSuccess
};


class Generator {
public:
    Generator();

    void Init(const Ruleset& rules, int chunkWidth, int chunkHeight, int numChunkWidth, int numChunkHeight);
    void Next();

    int GetCellTileId(int coordinates) const;
    const Cell& GetCell(int coordinates) const;
    const Ruleset& GetRuleset() const;
    int GetWidth() const;
    int GetHeight() const;

    void Render();

    ~Generator();
    
private:
    Image generatedImage;
    Texture2D generatedTexture;
    Ruleset ruleset;
    int worldWidthAsPixels; 
    int worldHeightAsPixels;
    int regionWidthAsPixels;
    int regionHeightAsPixels;
    int worldWidthAsRegions;
    int worldHeightAsRegions;
    int xRegionOfWorld;
    int yRegionOfWorld;

    GenerationState generationState;

    std::vector<Cell> cells;
    std::vector<bool> isRegionsGenerated;
    std::vector<int> regionsCoords;
    std::vector<int> regionsFailures;
    int currentRegionId;

    Heap<int> cellEntropyPriorityQueue = Heap<int>([this](const int& entropyA, const int& entropyB) -> bool {        
       return entropyA <= entropyB;
    }); // stores index of next cells to solve

    void Propagate(int beginCoordinates);
    void Expand(int coordinates, std::vector<int>& queueCoordinates, std::vector<bool>& isInQueue);
    void ContraintAdjacent(int adjacentCoordinates, TileDirection direction, const std::vector<int>& tilePossibilitiesIds, std::vector<int>& queueCoordinate, std::vector<bool>& isInQueue);
    void BuildCurrentRegion();
    void BuildInitialRegion();
    void StartNextRegion();
    void GenerateNextCell();

    void BacktrackRegions();
    void ResetRegion(int xRegion, int yRegion);
};
