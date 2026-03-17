#pragma once

#include <cstdint>
#include <vector>

#include <raylib.h>

#include "Heap.hpp"
#include "Ruleset.hpp"


enum SpecialCellType {
    NoSolution = -1,
    Unexplored = -2
};


class Cell {
public:
    Cell(const Ruleset& ruleset);

    bool Intersect(const std::vector<uint64_t>& otherPossibilities);
    int Collapse(const Ruleset& ruleset);
    int GetEntropy() const;
    int GetResultTile() const;

    const std::vector<uint64_t>& GetTilePossibilities() const;

private:
    std::vector<uint64_t> tilePossibilities;
    int globalFrequency;
    int resultTileId;
    int numberOfPossibleTiles;
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
    int numberOfReset;
    int maxNumberOfReset;

    bool regionGenerationFailure;

    std::vector<Cell> cells;
    std::vector<bool> regionsGenerated;

    Heap<int> cellEntropyPriorityQueue = Heap<int>([this](const int& entropyA, const int& entropyB) -> bool {        
       return entropyA <= entropyB;
    }); // stores index of next cells to solve

    void CompletePropagation(int beginCoordinates);
    void Propagate(int coordinates, std::vector<int>& queueCoordinates, std::vector<bool>& isInQueue);
    void ExpandAdjacent(int adjacentCoordinates, TileDirection direction, const Cell& cell, std::vector<int>& queueCoordinate, std::vector<bool>& isInQueue);
    void BuildRegion();
    void BuildInitialRegion();

};
