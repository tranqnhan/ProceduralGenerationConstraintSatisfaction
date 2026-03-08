#pragma once

#include <cstdint>
#include <vector>


struct Item {
    uint32_t color;
    int sideBranch;

    Item(uint32_t color, int sideBranch = -1) {
        this->color = color;
        this->sideBranch = sideBranch;
    };

    Item() {
        this->sideBranch = -1;
    };
};


struct Kernel {
    std::vector<Item> leafs;
    int frequencyIndex;

    Kernel(std::vector<Item>&& leafs, int frequencyIndex) {
        this->frequencyIndex = frequencyIndex;
        this->leafs = std::move(leafs);
    }
};


// Datastructure to map the kernels to an integer (the order seen)
class Composite {
public:
    void AddKernel(const std::vector<uint32_t>& kernel);
    int GetFrequency(int kernelBranch) const;
    const std::vector<Kernel>& GetBranches() const;

private:
    std::vector<int> globalFrequencies;
    std::vector<Kernel> branches; 

    // Returns index if kernel exists, -1 if not
    int IsKernelExists(const std::vector<uint32_t>& kernel); 
};