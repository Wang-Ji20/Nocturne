// arbitary precision(bit) integer parser(big endian or little endian)
// char[(bit+7)/8] -> uint64_t
// only some static functions

#pragma once
#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

// FIXME
[[maybe_unused]] static auto
bitsToNum(const char* bits, int bit, bool isLittleEndian) -> uint64_t {
    uint64_t ret = 0;
    if (isLittleEndian) {
        for (int i = 0; i < bit; i++) {
            ret |= ((bits[i / 8] >> (7 - (i % 8))) & 1) << i;
        }
    } else {
        for (int i = 0; i < bit; i++) {
            // debug info
            // std::cout << "i: " << i << " bit: " << bit << " bits[i/8]: " << (int)bits[i/8] << " bits[i/8] >> (7 - (i % 8)): " << (int)(bits[i/8] >> (7 - (i % 8))) << " (bits[i/8] >> (7 - (i % 8))) & 1: " << ((bits[i/8] >> (7 - (i % 8))) & 1) << " (bit - i - 1): " << (bit - i - 1) << std::endl;
            ret |= ((bits[i / 8] >> (7 - (i % 8))) & 1) << (bit - i - 1);
        }
    }
    return ret;
}

[[maybe_unused]] static auto
numToBits(uint64_t num, char* bits, int bit, bool isLittleEndian) -> void {
    if (isLittleEndian) {
        for (int i = 0; i < bit; i++) {
            bits[i / 8] |= ((num >> i) & 1) << (i % 8);
        }
    } else {
        for (int i = 0; i < bit; i++) {
            bits[i / 8] |= ((num >> (bit - i - 1)) & 1) << (i % 8);
        }
    }
}
