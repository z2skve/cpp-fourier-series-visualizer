#ifndef UI_ELEMENTS_H_
#define UI_ELEMENTS_H_

#include <SFML/Graphics.hpp>
#include <functional>

namespace ui {

void setupText(sf::Text& t, unsigned int size, sf::Vector2f pos);

class Slider{
    public:
        Slider(sf::Vector2f position, float length, float min_value, float max_value);

        void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);

        void Draw(sf::RenderWindow& window) const;

        float GetValue() const;
        void ResetValue();

        void SetMaxValue(float max_value) { max_value_ = max_value;}


    private:
        sf::RectangleShape track_;
        sf::CircleShape handle_;
        float min_value_;
        float max_value_;
        bool is_dragging_ = false;

};

class Panel {
    public:
        Panel(sf::Vector2f position, sf::Vector2f size, sf::Color color = sf::Color(30, 30, 30));

        void Draw(sf::RenderWindow& window) const; 

    private:
        sf::RectangleShape background_;
};

class TextBox{
    public:
        TextBox(sf::Vector2f position, sf::Vector2f size, int text_size, const sf::Font& font, sf::Color color = sf::Color(50, 50, 50));

        void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);

        void Draw(sf::RenderWindow& window) const;

        std::string GetText() const;
        void SetText(const std::string& text);

        bool IsFocused() const { return is_focused_; }
        bool IsReadyToDraw() const { return ready_to_draw_; }
        void ResetReadyToDraw() { ready_to_draw_ = false; }

    private:
        sf::RectangleShape box_;
        sf::Text display_text_;
        std::string content_;
        bool is_focused_ = false;
        bool ready_to_draw_ = false;

        const sf::Color kColorFocused = sf::Color(60, 60, 60);
        const sf::Color kColorUnfocused = sf::Color(30, 30, 30);

};

class HarmonicScreen {
    public:
        HarmonicScreen(sf::Vector2f position, sf::Vector2f size, std::vector<std::function<float(float)>> harmonics, sf::Color color = sf::Color(30, 30, 30));

        void Draw(sf::RenderWindow& window) const;

        void UpdateHarmonicIndex(int index);

        void SetHarmonics(const std::vector<std::function<float(float)>>& new_harmonics);

    private:
        void RecalculateVertices();
        std::vector<sf::Vertex> CalculateFunctionVertices(std::function<float(float)> func);

        sf::RectangleShape background_;
        int current_harmonic_index_ = 0;
        const int number_divisions_ = 16;
        const float math_range_x_ = 16.0f;
        
        std::array<sf::Vertex, 2> line_vertices_;
        std::vector<sf::Vertex> grid_lines_;
        std::vector<sf::Vertex> func_vertices_;

        float kTickSize_ = 2.0f;
        float kTickSpacing_;

        sf::Vector2f position_;
        sf::Vector2f size_;
        std::vector<std::function<float(float)>> harmonics_;
};
} // namespace ui
#endif  // UI_ELEMENTS_H_