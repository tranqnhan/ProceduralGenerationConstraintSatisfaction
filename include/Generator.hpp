#pragma once

#include <cstdint>
#include <vector>
#include <queue>

#include "raylib.h"

#include "Heap.hpp"
#include "Ruleset.hpp"

class Cell {
public:
    Cell(const Ruleset& ruleset);

    bool Intersect(const std::vector<uint64_t>& otherPossibilities);
    int Collapse();
    int GetEntropy() const;
    int GetSolvedTile() const;

    const std::vector<uint64_t>& GetTilePossibilities() const;

private:
    std::vector<uint64_t> tilePossibilities;
    int globalFrequency;
    int solvedTileId;
};


class Generator {
public:
    Generator();

    Image GenerateImage(const Ruleset& rule, int width, int height);

    Image debugImage;
    Texture2D debugTexture;
    Ruleset ruleset;
    int width; 
    int height;

    std::vector<Cell> cells;

    Heap<int> cellEntropyPriorityQueue = Heap<int>([this](const int& entropyA, const int& entropyB) -> bool {        
       return entropyA > entropyB;
    }); // stores index of next cells to solve

    // std::vector<std::pair<int, int>> debugOpenSet;

    void Init(const Ruleset& rules, int width, int height);
    void Next();

    void CompletePropagation(int beginCoordinates);
    void Propagate(int coordinates, std::queue<int>& queueCoordinates, ankerl::unordered_dense::set<int>& propagatedCoordinates, const ankerl::unordered_dense::set<int>& exploredCoordinates);
    void ExpandAdjacent(int adjacentCoordinates, TileDirection direction, const Cell& cell, std::queue<int>& queueCoordinates, ankerl::unordered_dense::set<int>& propagatedCoordinates, const ankerl::unordered_dense::set<int>& exploredCoordinates);
    
private:
    int GetEntropy(uint32_t constraints) const;
    uint32_t AddConstraints(uint32_t oldContraints, uint32_t newConstraints) const;
    uint32_t ResolveContraints(uint32_t contraints); // Reduce contraints to 1 solution 

};
