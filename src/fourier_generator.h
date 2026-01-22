#ifndef FOURIER_GENERATOR_H_
#define FOURIER_GENERATOR_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>

namespace fourier_sim {

class Generator {

    public:
        std::vector<sf::Vertex> GetUniversalFourier(int harmonics, int slices, std::function<float(float)> target_func, float range_start = 0.0f, float range_end = 16.0f);

        const std::vector<std::function<float(float)>>& GetHarmonics() const { 
            return all_harmonics_; 
        }

    private:
        std::vector<std::function<float(float)>> all_harmonics_;

};
} // namespace fourier_sim

#endif  // FOURIER_GENERATOR_H_