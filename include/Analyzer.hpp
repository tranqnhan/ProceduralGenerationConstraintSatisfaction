#pragma once

#include <string>

#include "raylib.h"

#include "Composite.hpp"
#include "Ruleset.hpp"

class Analyzer {
public:
    void AnalyzeImage(const std::string& imageFile, int expand);

    Image debugImage;
    Texture2D debugTexture;

private:
    int GetModulusSpaceCoord(int coord, int maxCoord) const;
    void AddKernelComposite(int x, int y, int width, int height, int length, Color *colors, Composite& composites);

    void DebugGenerateTexture(const Composite& compositeTree, int width, int height, int length);
};

