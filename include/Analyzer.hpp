#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include "CompositeTree.hpp"
#include "raylib.h"

#include "Ruleset.hpp"

class Analyzer {
public:
    void AnalyzeImage(const std::string& imageFile, int expand);

    Image debugImage;
    Texture2D debugTexture;

private:
    int GetModulusSpaceCoord(int coord, int maxCoord) const;
    void AddKernelComposite(int x, int y, int width, int height, int length, Color *colors, CompositeTree& composites);

    void DebugGenerateTexture(const CompositeTree& compositeTree, int width, int height, int length);
};

