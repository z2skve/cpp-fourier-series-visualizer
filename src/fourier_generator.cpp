#include "fourier_generator.h"
#include <cmath>

namespace fourier_sim {

std::vector<sf::Vertex> Generator::GetUniversalFourier(int harmonics, int slices, std::function<float(float)> target_func, float range_start, float range_end){
    const float kPixelsPerUnit = 50.f;
    const float kPi = 3.1415926535f;
    const float kUnit = 1.0f / kPixelsPerUnit;

    const float T = range_end - range_start;
    const float L = T / 2.0f;

    const float kDeltaX = T / static_cast<float>(slices);

    std::vector<float> an(harmonics + 1, 0.f);
    std::vector<float> bn(harmonics + 1, 0.f);

    all_harmonics_.clear();

    for (int n = 0; n <= harmonics; ++n){
        float sum_a = 0.f;
        float sum_b = 0.f;
        for (int i = 0; i < slices; ++i){
            float x_math = range_start + i * kDeltaX;
            float f_x = target_func(x_math);

            float angle = n * kPi * x_math / L;
            sum_a += f_x * std::cos(angle) * kDeltaX;
            sum_b += f_x * std::sin(angle) * kDeltaX;
        }
        an[n] = sum_a / L;
        bn[n] = sum_b / L;

        // Store individual harmonic functions
        float val_an = an[n];
        float val_bn = bn[n];

        if (n == 0) {
            all_harmonics_.push_back([val_an](float x) { 
                return val_an / 2.0f; 
            });
        } else {
            all_harmonics_.push_back([val_an, val_bn, n, kPi, L](float x) {
                float angle = n * kPi * x / L;
                return val_an * std::cos(angle) + val_bn * std::sin(angle);
            });
        }
    }

    std::vector<sf::Vertex> vertices;
    for (float x_math = range_start; x_math <= range_end; x_math += kUnit){
        float y_fourier = 0.0f;
        for (const auto& harmonic_func : all_harmonics_) {
            y_fourier += harmonic_func(x_math);
        }

        float x_pixels = x_math * kPixelsPerUnit;
        float y_pixels = y_fourier * kPixelsPerUnit;

        sf::Vertex point;
        point.position = {x_pixels, y_pixels};
        point.color = sf::Color::Yellow;
        vertices.push_back(point);
    }

    return vertices;

} 

} // namespace fourier_sim