#pragma once

#include <cstdint>
#include <vector>
#include <array>


enum TileDirection {
    NORTH,
    SOUTH,
    WEST,
    EAST,
    NUM_DIRECTIONS
};


class AdjacentTile {
public:
    AdjacentTile();
    AdjacentTile(int id, int localFrequency);

    int GetTileId() const;
    int GetLocalFrequency() const;
private:
    int id;
    int localFrequency;
};


class Tile {
public:
    Tile();
    Tile(uint32_t color, int globalFrequency);
    
    void SetAdjacentTiles(std::vector<AdjacentTile>&& adjacentTiles, TileDirection direction);
    const std::vector<AdjacentTile>& GetAdjacentTiles(TileDirection direction) const;

    uint32_t GetColor() const;

private:
    uint32_t color;
    std::array<std::vector<AdjacentTile>, TileDirection::NUM_DIRECTIONS> adjacentTiles;
    int globalFrequency;
};


class Ruleset {
public:
    Ruleset(int numTiles);

    void SetAdjacentTiles(int tileId, TileDirection direction, const std::vector<int>& adjacentTileIds, const std::vector<int>& adjacentTileFrequencies);

    const Tile& GetTile(int tileId) const;
    const std::vector<AdjacentTile>& GetAdjacentTiles(int tileId, TileDirection direction) const;
    const uint32_t GetTileColor(int tileId) const;

private:
    std::vector<Tile> tiles;
};
