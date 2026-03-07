#pragma once

#define MAX_DISTINCT_OBJECTS 32 // Maximum number of distinct objects to constraint

#include <cstdint>
#include <vector>
#include <array>

class Tile {
public:
    Tile(uint32_t color);
    void AddAdjacentTile(int adjacentTileId, int direction);

private:
    uint32_t color;
    std::array<std::vector<int>, 4> adjacentTiles;
    std::array<std::vector<int>, 4> frequencies;

};


class Ruleset {
public:

    Ruleset(int size);

    // Returns the tile id
    int AddTile(uint32_t color);
    int AddAdjacentTile(int tileId, int adjacentTileId, int direction);

private:
    std::vector<Tile> tiles;
};
