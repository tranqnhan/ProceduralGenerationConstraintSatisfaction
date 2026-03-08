#include "Ruleset.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>

AdjacentTile::AdjacentTile() {
    this->id = 0;
    this->localFrequency = 0;
}

AdjacentTile::AdjacentTile(int id, int localFrequency) {
    this->id = id;
    this->localFrequency = localFrequency;
}


int AdjacentTile::GetTileId() const {
    return this->id;
}

int AdjacentTile::GetLocalFrequency() const {
    return this->localFrequency;
}

Tile::Tile() {
    this->color = 0;
    this->globalFrequency = 0;
}

Tile::Tile(uint32_t color, int globalFrequency) {
    this->color = color;
    this->globalFrequency = globalFrequency;
}

void Tile::SetAdjacentTiles(std::vector<AdjacentTile>&& adjacentTiles, TileDirection direction) {
    std::sort(adjacentTiles.begin(), adjacentTiles.end(), [](const AdjacentTile& a, const AdjacentTile b) {
        return a.GetTileId() < b.GetTileId();
    });
    this->adjacentTiles[direction] = std::move(adjacentTiles);
}

const std::vector<AdjacentTile>& Tile::GetAdjacentTiles(TileDirection direction) const {
    return this->adjacentTiles[direction];
}

Ruleset::Ruleset(int numTiles) {
    this->tiles.resize(numTiles);
}


void Ruleset::SetAdjacentTiles(int tileId, TileDirection direction, const std::vector<int>& adjacentTileIds, const std::vector<int>& adjacentTileFrequencies) {
    if (tileId >= this->tiles.size()) {
        std::cerr << "Tile " << tileId << " index is out of bound of " << this->tiles.size() << "." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (adjacentTileIds.size() != adjacentTileFrequencies.size()) {
        std::cerr << "Adjacent arrays' sizes do not match." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::vector<AdjacentTile> adjacentTiles(adjacentTileIds.size());

    for (int i = 0; i < adjacentTiles.size(); ++i) {
        if (adjacentTileIds[i] >= this->tiles.size()) {
            std::cerr << "Adjacent tile " << adjacentTileIds[i] << " index is out of bound of " << this->tiles.size() << "." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        adjacentTiles[i] = AdjacentTile(adjacentTileIds[i], adjacentTileFrequencies[i]);
    } 

    this->tiles[tileId].SetAdjacentTiles(std::move(adjacentTiles), direction);
}


const Tile& Ruleset::GetTile(int tileId) const {
    return this->tiles[tileId];
}


const std::vector<AdjacentTile>& Ruleset::GetAdjacentTiles(int tileId, TileDirection direction) const {
    return this->tiles[tileId].GetAdjacentTiles(direction);
}

const uint32_t Ruleset::GetTileColor(int tileId) const {
    return this->tiles[tileId].GetColor();
}
