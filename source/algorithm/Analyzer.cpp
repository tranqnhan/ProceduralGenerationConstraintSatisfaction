#include "raylib.h"
#include <cstdio>

#include "Analyzer.hpp"

Ruleset Analyzer::AnalyzeImage(const std::string &imageFile) {
    Ruleset ruleset;

    // Load image from filename
    Image sampleImage = LoadImage(imageFile.c_str());    
    ImageFormat(&sampleImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color *colors = (Color *) sampleImage.data;

    std::unordered_map<uint32_t, int> colorMap(MAX_DISTINCT_OBJECTS * 2);

    for (int i = 0; i < sampleImage.height; ++i) {
        for (int j = 0; j < sampleImage.width; ++j) {
            const Color centerColor = colors[i * sampleImage.height + j];
            const int centerId = GetColorID(centerColor, ruleset, colorMap);

            const int boundMinY = (i - 1) > 0 ? (i - 1) : 0;
            const int boundMinX = (j - 1) > 0 ? (j - 1) : 0;
            const int boundMaxY = (i + 2) > sampleImage.height ? sampleImage.height : (i + 2);
            const int boundMaxX = (j + 2) > sampleImage.width ? sampleImage.width : (j + 2);
            
            // Look at neighbors
            int direction = 0;
            for (int k = boundMinY; k < boundMaxY; ++k) {
                for (int l = boundMinX; l < boundMaxX; ++l) {
                    if (k == i && l == j) continue;
                    const Color neighborColor = colors[k * sampleImage.height + l];
                    const int neigborId = GetColorID(neighborColor, ruleset, colorMap);
                    ruleset.AddConstraint(centerId, neigborId, direction++);
                }
            }
        }
    }

    for (int i = 0; i < ruleset.GetNumberOfObjects(); ++i) {
        for (int k = 0; k < 8; ++k) {
            uint32_t constraint = ruleset.GetConstraints(i, k);
            std::printf("id %i constraint %b\n", i, constraint);
        }
    }

    UnloadImage(sampleImage);

    return ruleset;
}

int Analyzer::GetColorID(const Color& color, Ruleset& ruleset, std::unordered_map<uint32_t, int>& colorMap) {
    const uint32_t value = (color.r << 16) | (color.b << 8) | (color.g);
    auto iterator = colorMap.find(value);
    if (iterator == colorMap.end()) {
        ruleset.AddColor(color);
        colorMap.emplace(value, ruleset.GetNumberOfObjects() - 1);
        return ruleset.GetNumberOfObjects() - 1;
    }

    return iterator->second;
}
