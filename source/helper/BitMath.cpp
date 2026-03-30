#include "BitMath.hpp"

std::vector<int> BitMath::GetSetPositions(const std::vector<uint64_t>& bitsets) {
    std::vector<int> result;

    uint64_t mask = uint64_t(1) << 63;

    for (int i = 0; i < bitsets.size(); i++) {
        if (bitsets[i] == 0) continue;

        const uint64_t& bitset = bitsets[i];
        
        mask = uint64_t(1) << 63;
        
        for (int j = 0; j < 64; ++j) {
            if (mask & bitset) {
                const int position = i * 64 + j;
                result.emplace_back(position);
            }

            mask >>= 1;
        }
    }

    return result;
}


void BitMath::GetSetPositions(std::vector<int>& result, const std::vector<uint64_t>& bitsets) {
    result.clear();

    uint64_t mask = uint64_t(1) << 63;

    for (int i = 0; i < bitsets.size(); i++) {
        if (bitsets[i] == 0) continue;

        const uint64_t& bitset = bitsets[i];
        
        mask = uint64_t(1) << 63;
        
        for (int j = 0; j < 64; ++j) {
            if (mask & bitset) {
                const int position = i * 64 + j;
                result.emplace_back(position);
            }

            mask >>= 1;
        }
    }
}