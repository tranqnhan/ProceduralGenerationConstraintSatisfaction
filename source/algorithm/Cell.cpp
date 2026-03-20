#include "BitMath.hpp"
#include "XorshiftRandom.hpp"
#include "Cell.hpp"

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
    for (int i = 0; i < this->tilePossibilities.size(); ++i) {
        this->tilePossibilities[i] = ~uint64_t(0);
    }

    uint64_t& tileSet = this->tilePossibilities.back();
    tileSet <<= 64 - (ruleset.GetNumberOfTiles() % 64);

    this->globalFrequency = 0;
    this->resultTileId = SpecialCellType::Unexplored;
    this->numberOfPossibleTiles = ruleset.GetNumberOfTiles();
}
