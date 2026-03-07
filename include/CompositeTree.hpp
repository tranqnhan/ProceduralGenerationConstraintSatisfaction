#pragma once

#include <cstdint>
#include <vector>


struct PixelLeaf {
    uint32_t color;
    int sideBranch;

    PixelLeaf(uint32_t color, int sideBranch = -1) {
        this->color = color;
        this->sideBranch = sideBranch;
    };

    PixelLeaf() {
        this->sideBranch = -1;
    };
};


struct KernelBranch {
    std::vector<PixelLeaf> leafs;
    int frequencyIndex;

    KernelBranch(std::vector<PixelLeaf>&& leafs, int frequencyIndex) {
        this->frequencyIndex = frequencyIndex;
        this->leafs = std::move(leafs);
    }
};


// Datastructure to map the kernels to an integer (the order seen)
class CompositeTree {
public:
    void AddKernel(const std::vector<uint32_t>& kernel);
    int GetFrequency(int kernelBranch) const;
    const std::vector<KernelBranch>& GetBranches() const;

private:
    std::vector<int> frequencies;
    std::vector<KernelBranch> branches; 

    // Returns index if kernel exists, -1 if not
    int IsKernelExists(const std::vector<uint32_t>& kernel); 
};