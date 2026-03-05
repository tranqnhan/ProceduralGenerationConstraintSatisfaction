#include "raylib.h"
#include <array>
#include <cstdio>
#include <print>
#include <vector>

#include "Analyzer.hpp"

int Analyzer::GetModulusSpaceCoord(int coord, int maxCoord) const {
    if (coord < 0) {
        coord = maxCoord + coord + 1;
    } else if (coord > maxCoord) {
        coord = coord - maxCoord - 1;
    }

    return coord;
}

void Analyzer::AddKernelComposite(int x, int y, int width, int height, int length, Color *colors, std::vector<Color>& composites) {
    // Kernels

    for (int k = 0; k < length; ++k) {
        for (int l = 0; l < length; ++l) {
            const int transformCoordY = GetModulusSpaceCoord(y + k, height - 1);
            const int transformCoordX = GetModulusSpaceCoord(x + l, width - 1);
          //  printf("x %i y %i tx %i ty %i\n", x + l, y + k, transformCoordX, transformCoordY);
            composites.emplace_back(colors[transformCoordY * height + transformCoordX]);
        }
    }
}


void Analyzer::AnalyzeImage(const std::string &imageFile, int length) {
    // Load image from filename
    Image sampleImage = LoadImage(imageFile.c_str());    
    ImageFormat(&sampleImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color *colors = (Color *) sampleImage.data;

    const int height = sampleImage.height;
    const int width = sampleImage.width;

    std::vector<Color> composites;
    
    for (int i = -length + 1; i < height; ++i) {
        for (int j = -length + 1; j < width; ++j) {
            AddKernelComposite(j, i, width, height, length, colors, composites);
        }
    }

    UnloadImage(sampleImage);

    DebugGenerateTexture(composites, width, height, length);
}

void Analyzer::DebugGenerateTexture(const std::vector<Color> composites, int width, int height, int length) {
    //debugImage = GenImageColor(composites.size(), 1, BLACK);
    //Color *debugColors = (Color *) debugImage.data;
    //for (int i = 0; i < composites.size(); ++i) {
    //    debugColors[i] = composites[i];
    //}
    
    //Debugging
    const int debugImageWidth = (width + length - 1) * length;
    const int debugImageHeight = (height + length - 1) * length;
    

    debugImage = GenImageColor(debugImageWidth, debugImageHeight, WHITE);
    Color *debugColors = (Color *) debugImage.data;
    
    // const int numComposites = composites.size() / (length * length);
    
    // for (int i = 0; i < numComposites; ++i) {
    //    for (int k = 0; k < length; ++k) {
    //        for (int l = 0; l < length; ++l) {
    //            debugColors[(k * length + l) * (i + 1)] = composites[(i + 1) * (k + 1) * (l + 1) - 1];
    //        }            
    //    }
    // }

    int atX = 0;
    int atY = 0;

    for (int j = 0; j < debugImageHeight; j += length) {
        for (int k = 0; k < (debugImageWidth * length); k += (length * length)) {
            for (int i = k; i < k + (length * length); ++i) {
                const Color colorAt = composites[j * debugImageWidth + i];
                const int x = ((i - k) % length) + atX;
                const int y = int((i - k) / length) + atY;
                debugColors[y * debugImageWidth + x] = colorAt;
            }
            atX += length;
        }
        atY += length - 1;
    }

    printf("%i", debugImageWidth);
    
    //int m = 0;
    //
    //const int sectionWidth = (width + length - 1);
    //int h = 1;
    //
    //for (int i = 0; i < length; ++i) {
    //    for (int k = i * length; k < (i + 2) * debugImageWidth; k += length * length) {
    //        for (int j = k; j < k + length; ++j) {
    //            debugColors[m++] = composites[j];
    //        }
    //    }
    //}

    debugTexture = LoadTextureFromImage(debugImage);
    UnloadImage(debugImage);
}