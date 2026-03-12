#include <cstdint>

#include "Ruleset.hpp"


Tile::Tile() {
    this->color = 0;
    this->globalFrequency = 0;
}


Tile::Tile(uint32_t color, int globalFrequency, int numberOfTiles) {
    this->color = color;
    this->globalFrequency = globalFrequency;
}


void Tile::SetColor(uint32_t color) {
    this->color = color;
}


uint32_t Tile::GetColor() const {
    return this->color;
}


int Tile::GetGlobalFrequency() const {
    return this->globalFrequency;
}

void Tile::SetGlobalFrequency(int globalFrequency) {
    this->globalFrequency = globalFrequency;
}


void Tile::SetAdjacentTiles(std::vector<uint64_t>&& adjacentTilesInDirection, int direction) {
    this->adjacentTiles[direction] = std::move(adjacentTilesInDirection);
}


const std::vector<uint64_t>& Tile::GetAdjacentTiles(int direction) const {
    return this->adjacentTiles[direction];
}

//TODO: remove temporary constructor
Ruleset::Ruleset() {
    
}


Ruleset::Ruleset(int numTiles) {
    this->tiles.resize(numTiles);
}


int Ruleset::GetNumberOfTiles() const {
    return this->tiles.size();
}


const std::vector<Tile>& Ruleset::GetTiles() const {
    return this->tiles;
}


void Ruleset::SetTileColor(int tileId, uint32_t color) {
    this->tiles[tileId].SetColor(color);
}


void Ruleset::SetTileFrequency(int tileId, int globalFrequency) {
    this->tiles[tileId].SetGlobalFrequency(globalFrequency);
}


void Ruleset::SetAdjacentTiles(int tileId, int direction, const std::vector<int>& adjacentTileIds, const std::vector<int>& adjacentTileFrequencies) {

    std::vector<uint64_t> adjacentTiles(int(this->GetNumberOfTiles() / 64) + 1);

    for (const int tileId : adjacentTileIds) {
        const int valueIndex = int(tileId / 64);
        const int bitIndex = tileId - (valueIndex * 64);
        adjacentTiles[valueIndex] |= (uint64_t(1) << (63 - bitIndex));
    } 

    this->tiles[tileId].SetAdjacentTiles(std::move(adjacentTiles), direction);
}


const Tile& Ruleset::GetTile(int tileId) const {
    return this->tiles[tileId];
}


const std::vector<uint64_t>& Ruleset::GetAdjacentTiles(int tileId, int direction) const {
    return this->tiles[tileId].GetAdjacentTiles(direction);
}

const uint32_t Ruleset::GetTileColor(int tileId) const {
    return this->tiles[tileId].GetColor();
}