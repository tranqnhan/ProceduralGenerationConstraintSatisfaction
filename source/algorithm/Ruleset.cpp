#include "Ruleset.hpp"

#include <cstdint>

Tile::Tile(uint32_t color) {
    this->color = color;
}



int Ruleset::AddTile(uint32_t color) {
    tiles.emplace_back(color);
    return tiles.size() - 1;
}

void Ruleset::AddAdjacentTile(int tileId, int adjacentTileId, int direction) {
    tiles[tileId].AddAdjacentTile(adjacentTileId, direction);
}