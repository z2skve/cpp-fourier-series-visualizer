#include "function_generator.h"
#include <cmath>

namespace eq_sim {
    const int kWidth = 800;
    const int kHeight = 600;

    const float kPixelsPerUnit = 50.f;


    std::vector<sf::Vertex> getInstance(std::function<float(float)> target_func){
        std::vector<sf::Vertex> vertices;

        for (float x = 0.0f; x <= kWidth; x += 1.0f) {
            float x_math = x / kPixelsPerUnit;
            float y_math = target_func(x_math);
            float y_pixels = y_math * kPixelsPerUnit;

            sf::Vertex point;
            point.position = {x, y_pixels};
            point.color = sf::Color::Green;
            vertices.push_back(point);
        }

        return vertices;
    }

}  // namespace eq_sim