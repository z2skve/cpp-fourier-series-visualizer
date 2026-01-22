#ifndef FUNCTION_GENERATOR_H_
#define FUNCTION_GENERATOR_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>

namespace eq_sim {
    std::vector<sf::Vertex> getInstance(std::function<float(float)> target_func);
} // namespace eq_sim


#endif  // FUNCTION_GENERATOR_H_