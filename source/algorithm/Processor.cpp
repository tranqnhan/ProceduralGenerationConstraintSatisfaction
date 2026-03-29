#include <vector>

#include <raylib.h>

#include "CompressColor.hpp"
#include "Ruleset.hpp"
#include "Composite.hpp"
#include "Processor.hpp"

int Processor::GetModulusSpaceCoord(int coord, int maxCoord) const {
    if (coord < 0) {
        coord = maxCoord + coord + 1;
    } else if (coord > maxCoord) {
        coord = coord - maxCoord - 1;
    }

    return coord;
}


int Processor::ProcessKernel(int x, int y, int width, int height, int kernelLength, Color *colors, Composite& compositeTree) {
    // Kernels
    std::vector<uint32_t> kernel(kernelLength * kernelLength);

    for (int k = 0; k < kernelLength; ++k) {
        for (int l = 0; l < kernelLength; ++l) {
            const int transformCoordY = GetModulusSpaceCoord(y + k, height - 1);
            const int transformCoordX = GetModulusSpaceCoord(x + l, width - 1);
            const Color color = colors[transformCoordY * width + transformCoordX];
            const uint32_t compressedColor = CompressColor::Compress(color);
            kernel[k * kernelLength + l] = compressedColor;
        }
    }

    return compositeTree.NextKernel(std::move(kernel));
}


Ruleset Processor::AnalyzeImage(Color *colors, int width, int height, int length) {
    const int compositeHeight = height;
    const int compositeWidth = width;

    Composite composite(length);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            ProcessKernel(j, i, width, height, length, colors, composite);
        }
    }

    // Get adjacent kernels
    for (int i = 0; i < composite.GetNumberOfKernels(); ++i) {
        Kernel& primaryKernel = composite.GetKernel(i);
        for (int j = i; j < composite.GetNumberOfKernels(); ++j) {
            Kernel& secondaryKernel = composite.GetKernel(j);

            for (int direction = 0; direction < TileDirection::NUM_DIRECTIONS; ++direction) {
                const int opposeDirection = (~direction) & 0b11;
                if (primaryKernel.CompareAdjacentOverlap(secondaryKernel, direction, opposeDirection)) {
                    primaryKernel.AddAdjacency(j, direction);
                    secondaryKernel.AddAdjacency(i, opposeDirection);
                }
            }
        }
    }
    
    // Translate to Ruleset
    Ruleset ruleset(composite.GetNumberOfKernels());
    std::vector<Kernel>& kernels = composite.GetKernels();

    for (int i = 0; i < composite.GetNumberOfKernels(); ++i) {
        Kernel& kernel = kernels[i];
        ruleset.SetTileFrequency(i, kernel.globalFrequency);
        ruleset.SetTileColor(i, std::move(kernel.leafs));
        for (int d = 0; d < TileDirection::NUM_DIRECTIONS; ++d) {
            const int adjacentSize = kernel.adjacentKernelFrequencies[d].size();
            std::vector<int> adjacentTileIds(adjacentSize);
            std::vector<int> adjacentTileFrequencies(adjacentSize);
            int v = 0;
            for (const auto& iteAdjacentTile : kernel.adjacentKernelFrequencies[d]) {
                adjacentTileIds[v] = iteAdjacentTile.first;
                adjacentTileFrequencies[v] = iteAdjacentTile.second;
                v++;
            }
            ruleset.SetAdjacentTiles(i, d, adjacentTileIds, adjacentTileFrequencies);
        }
    }
    
    //DebugGenerateTexture(composite, width, height, length);

    return ruleset;
}
