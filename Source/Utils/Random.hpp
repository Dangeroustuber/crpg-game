#pragma once

#include <random>
#include <chrono>

namespace Random {
    inline std::mt19937& generator() {
        static std::mt19937 gen((unsigned)(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
        return gen;
    }

    inline float randomFloat(float min = 0.0f, float max = 1.0f) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(generator());
    }

    inline double randomDouble(double min = 0.0, double max = 1.0) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(generator());
    }

    inline int randomInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(generator());
    }
}