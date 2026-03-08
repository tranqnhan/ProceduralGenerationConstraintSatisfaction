#include "Composite.hpp"

void Composite::AddKernel(const std::vector<uint32_t>& kernel) {
    const int kernelIndex = IsKernelExists(kernel);
    
    if (kernelIndex >= 0) {
        globalFrequencies[kernelIndex]++;
        return;
    }

    std::vector<Item> leafs;
    leafs.resize(kernel.size());
    for (int i = 0; i < kernel.size(); ++i) {
        leafs[i].color = kernel[i];
    }

    branches.emplace_back(std::move(leafs), globalFrequencies.size());
    globalFrequencies.emplace_back(1);
}

int Composite::GetFrequency(int kernelBranch) const {
    return globalFrequencies[kernelBranch];
}

int Composite::IsKernelExists(const std::vector<uint32_t>& kernel) {
    for (int i = 0; i < branches.size(); ++i) {
        if (kernel.size() != branches[i].leafs.size()) continue;
        int j;
        for (j = 0; j < branches[i].leafs.size(); ++j) {
            if (branches[i].leafs[j].color != kernel[j]) {
                break;
            }
        }
        if (j == branches[i].leafs.size()) {
            return i;
        }
    }

    return -1;
}


const std::vector<Kernel>& Composite::GetBranches() const {
    return this->branches;
}