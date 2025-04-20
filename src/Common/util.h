#pragma once
#include <random>

enum struct ConnectionSystem {
    CLIENT,
    SERVER
};

enum struct KeyType { 
    PUBLIC, 
    PRIVATE 
};

inline uint64_t getRandomUint64() {
    std::random_device rd;  // Seed generator
    std::mt19937_64 gen(rd()); // 64-bit Mersenne Twister PRNG
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

    return dist(gen);
}