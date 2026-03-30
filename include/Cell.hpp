#pragma once

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
    const std::vector<int>& GetTilePossibilitiesAsIds();
    void Clear(const Ruleset& ruleset);

private:
    std::vector<uint64_t> tilePossibilities;
    std::vector<int> tileIds;
    int globalFrequency;
    int resultTileId;
    int numberOfPossibleTiles;
    bool updateTileIds;
};

