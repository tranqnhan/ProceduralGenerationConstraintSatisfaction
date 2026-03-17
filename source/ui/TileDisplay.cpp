#include <cstdint>

#include <raylib.h>

#include "BitMath.hpp"
#include "CompressColor.hpp"
#include "Generator.hpp"
#include "Program.hpp"

#include "Ruleset.hpp"
#include "TileDisplay.hpp"


TileDisplay::TileDisplay() {}


void TileDisplay::Init() {
    this->pointerColor = Color {
        .r = 123,
        .g = 50,
        .b = 100,
        .a = 180
    };

    this->tileImage = GenImageColor(EXPAND, EXPAND, BLANK);
    this->tileTexture = LoadTextureFromImage(this->tileImage);
    this->uiSelectCell = false;
    this->uiSelectSameCellCount = 0;
    this->uiSelectCellCoords = -2;
    this->uiSelectTileId = -1;
}


void TileDisplay::Input() {
    this->pointerX = GetMouseX();
    this->pointerY = GetMouseY();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        this->uiSelectCell = true;
    }
}


void TileDisplay::Update(const Generator& generator) {
    if (this->uiSelectCell) {
        const int gridX = this->pointerX / PIXEL_SCALE;
        const int gridY = this->pointerY / PIXEL_SCALE;
        const int coords = gridY * generator.GetWidth() + gridX;
        const int tileId = generator.GetCellTileId(coords);
        
        if (coords == this->uiSelectCellCoords) {
            this->uiSelectSameCellCount++;
        } else {
            this->uiSelectSameCellCount = 0;
            this->uiSelectCellCoords = coords;
        }

        const Ruleset& ruleset = generator.GetRuleset();
 
        // coords 8522 observe 13

        if (tileId >= 0) {
            const std::vector<uint32_t>& compressedColors = ruleset.GetTileColor(tileId);
            Color *colors = (Color *)this->tileImage.data;
            for (int i = 0; i < compressedColors.size(); ++i) {
                colors[i] = CompressColor::Decompress(compressedColors[i]);
            }
            UpdateTexture(this->tileTexture, this->tileImage.data);
            this->uiSelectTileId = tileId;
        } else {    
            const Cell& cell = generator.GetCell(coords);
            const std::vector<int>& cellTileIds = BitMath::GetSetPositions(cell.GetTilePossibilities());

            if (cellTileIds.size() > 0) {
                const int nextTileId = cellTileIds[this->uiSelectSameCellCount % cellTileIds.size()];

                const std::vector<uint32_t>& compressedColors = ruleset.GetTileColor(nextTileId);
                Color *colors = (Color *)this->tileImage.data;
                for (int i = 0; i < compressedColors.size(); ++i) {
                    colors[i] = CompressColor::Decompress(compressedColors[i]);
                }
                UpdateTexture(this->tileTexture, this->tileImage.data);
                this->uiSelectTileId = nextTileId;
            }

        }

        //std::printf("coords %i tile id %i\n", coords, this->uiSelectTileId);

        this->uiSelectCell = false;
    }
}


void TileDisplay::Render() {
    DrawTextureEx(this->tileTexture, Vector2{.x = 0, .y = 0}, 0, PIXEL_SCALE*2, WHITE);

    const int snapGridX = this->pointerX - (this->pointerX % PIXEL_SCALE);
    const int snapGridY = this->pointerY - (this->pointerY % PIXEL_SCALE);
    DrawRectangle(snapGridX, snapGridY, PIXEL_SCALE, PIXEL_SCALE, this->pointerColor);    
}


TileDisplay::~TileDisplay() {
    UnloadImage(this->tileImage);
    UnloadTexture(this->tileTexture);
}