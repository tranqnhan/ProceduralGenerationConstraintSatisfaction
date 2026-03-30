#pragma once

#include <cstdint>
#include <vector>

class BitMath {
public:
    static std::vector<int> GetSetPositions(const std::vector<uint64_t>& bitsets);
    static void GetSetPositions(std::vector<int>& result, const std::vector<uint64_t>& bitsets);

};