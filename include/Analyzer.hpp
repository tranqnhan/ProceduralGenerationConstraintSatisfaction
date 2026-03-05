#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include "raylib.h"

#include "Ruleset.hpp"

class Analyzer {
public:
    void AnalyzeImage(const std::string& imageFile, int expand);

    Image debugImage;
    Texture2D debugTexture;

private:
    int GetModulusSpaceCoord(int coord, int maxCoord) const;
    void AddKernelComposite(int x, int y, int width, int height, int length, Color *colors, std::vector<Color>& composites);

    void DebugGenerateTexture(const std::vector<Color> composites, int width, int height, int length);
};

