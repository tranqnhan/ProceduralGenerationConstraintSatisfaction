#include <vector>

#include "raylib.h"

#include "Composite.hpp"

#include "Analyzer.hpp"

int Analyzer::GetModulusSpaceCoord(int coord, int maxCoord) const {
    if (coord < 0) {
        coord = maxCoord + coord + 1;
    } else if (coord > maxCoord) {
        coord = coord - maxCoord - 1;
    }

    return coord;
}

void Analyzer::AddKernelComposite(int x, int y, int width, int height, int length, Color *colors, Composite& compositeTree) {
    // Kernels
    std::vector<uint32_t> kernel;

    for (int k = 0; k < length; ++k) {
        for (int l = 0; l < length; ++l) {
            const int transformCoordY = GetModulusSpaceCoord(y + k, height - 1);
            const int transformCoordX = GetModulusSpaceCoord(x + l, width - 1);
          //  printf("x %i y %i tx %i ty %i\n", x + l, y + k, transformCoordX, transformCoordY);
            const Color color = colors[transformCoordY * height + transformCoordX];
            kernel.emplace_back(((color.r) << 16) | (color.g << 8) | (color.b));
        }
    }

    compositeTree.AddKernel(kernel);
}


void Analyzer::AnalyzeImage(const std::string &imageFile, int length) {
    // Load image from filename
    Image sampleImage = LoadImage(imageFile.c_str());    
    ImageFormat(&sampleImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color *colors = (Color *) sampleImage.data;

    const int height = sampleImage.height;
    const int width = sampleImage.width;

    Composite compositeTree;
    
    for (int i = -length + 1; i < height; ++i) {
        for (int j = -length + 1; j < width; ++j) {
            AddKernelComposite(j, i, width, height, length, colors, compositeTree);
        }
    }

    UnloadImage(sampleImage);

    DebugGenerateTexture(compositeTree, width, height, length);
}

void Analyzer::DebugGenerateTexture(const Composite& compositeTree, int width, int height, int length) {
    //Debugging
    const int debugImageWidth = (width + length - 1) * length;
    const int debugImageHeight = (height + length - 1) * length;
    
    debugImage = GenImageColor(debugImageWidth, debugImageHeight, WHITE);
    Color *debugColors = (Color *) debugImage.data;
    
    const std::vector<Kernel>& kernels = compositeTree.GetBranches();

    int atX = 0;
    int atY = 0;

    for (int k = 0; k < kernels.size(); ++k) {
        for (int i = 0; i < length; ++i) {
            for (int j = 0; j < length; ++j) {
                const uint32_t colorId = kernels[k].leafs[i * length + j].color;
                const Color color = Color{
                    .r = (unsigned char)((colorId & 0xFF0000) >> 16), 
                    .g = (unsigned char)((colorId & 0xFF00) >> 8),
                    .b = (unsigned char)((colorId & 0xFF)),
                    .a = 255
                };

                const int x = j + atX;
                const int y = i + atY;
                debugColors[y * debugImageWidth + x] = color;
            }
        }

        atX += length;
        if (atX >= debugImageWidth) {
            atX = 0;
            atY += length;
        }
    }

    debugTexture = LoadTextureFromImage(debugImage);
    UnloadImage(debugImage);
}