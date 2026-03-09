#pragma once

#include <cstdint>
#include <vector>
#include <array>

#include "ankerl/unordered_dense.h"

#include "Ruleset.hpp"

class Item {
public:
    uint32_t color;
    int sideKernel;

    Item(uint32_t color, int sideKernel = -1) {
        this->color = color;
        this->sideKernel = sideKernel;
    };

    Item() {
        this->sideKernel = -1;
    };
};


class Kernel {
public:
    std::vector<Item> leafs;
    int globalFrequency;
    std::array<ankerl::unordered_dense::map<int, int>, TileDirection::NUM_DIRECTIONS> adjacentKernelFrequencies;

    Kernel(std::vector<Item>&& leafs);

    void IncreaseGlobalFrequency();
    int GetGlobalFrequency() const;

};


// Datastructure to map the kernels to an integer (the order seen)
class Composite {
public:
    int AppendKernel(const std::vector<uint32_t>& kernel);
    int GetGlobalFrequency(int kernelId) const;
    const std::vector<Kernel>& GetBranches() const;
    Kernel& GetKernel(int kernelId);

private:
    std::vector<Kernel> kernels;

    // Returns index if kernel exists, -1 if not
    int IsKernelExists(const std::vector<uint32_t>& kernel); 
};