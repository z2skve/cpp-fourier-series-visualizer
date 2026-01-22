#include "ui_elements.h"
#include <algorithm>

namespace ui{

    void setupText(sf::Text& t, unsigned int size, sf::Vector2f pos) {
        t.setCharacterSize(size);
        t.setFillColor(sf::Color::White);
        t.setScale({1.0f, -1.0f}); 
        t.setPosition(pos);
        
        t.setOrigin({0.0f, t.getLocalBounds().size.y / 2.0f});
    }

    Slider::Slider(sf::Vector2f position, float lenght, float min_value, float max_value) 
        : min_value_(min_value), max_value_(max_value) {

        track_.setSize({lenght, 4.f});
        track_.setFillColor(sf::Color(100, 100, 100));
        track_.setOrigin({0.f, 2.f});
        track_.setPosition(position);

        handle_.setRadius(8.f);
        handle_.setFillColor(sf::Color::White);
        handle_.setOrigin({8.f, 8.f});
        handle_.setPosition(position);
    }

    void Slider::HandleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        sf::Vector2f mouse_pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (event.is<sf::Event::MouseButtonPressed>()){
            if (handle_.getGlobalBounds().contains(mouse_pos)) {
                is_dragging_ = true;
            }
        }

        if (event.is<sf::Event::MouseButtonReleased>()){
            is_dragging_ = false;
        }

        if (is_dragging_){
            float new_x = mouse_pos.x;
            float track_left = track_.getPosition().x;
            float track_right = track_left + track_.getSize().x;
            new_x = std::clamp(new_x, track_left, track_right);

            handle_.setPosition({new_x, handle_.getPosition().y});
        }
    }

    float Slider::GetValue() const {
        float track_left = track_.getPosition().x;
        float lenght = track_.getSize().x;
        float handle_relative_x = handle_.getPosition().x - track_left;

        float percentage = handle_relative_x / lenght;
        return min_value_ + percentage * (max_value_ - min_value_);
    }

    void Slider::Draw(sf::RenderWindow& window) const {
        window.draw(track_);
        window.draw(handle_);
    }

    void Slider::ResetValue() {
        handle_.setPosition({track_.getPosition().x, handle_.getPosition().y});
    }

    Panel::Panel(sf::Vector2f position, sf::Vector2f size, sf::Color color) {
        background_.setFillColor(color);
        background_.setSize(size);
        background_.setPosition(position);
    }

    void Panel::Draw(sf::RenderWindow& window) const {
        window.draw(background_);
    }

    TextBox::TextBox(sf::Vector2f position, sf::Vector2f size, int text_size, const sf::Font& font, sf::Color color) : display_text_(font) {
        box_.setFillColor(color);
        box_.setSize(size);
        box_.setPosition(position);
        box_.setOutlineColor(sf::Color::White);
        box_.setOutlineThickness(1.f);

        display_text_.setCharacterSize(text_size);
        display_text_.setFillColor(sf::Color::White);
        display_text_.setScale({1.f, -1.f});
        display_text_.setOrigin({0.f, display_text_.getLocalBounds().size.y / 2.f});
        display_text_.setPosition({position.x + 5.f, position.y + size.y});
    }

    void TextBox::HandleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        sf::Vector2f mouse_pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (event.is<sf::Event::MouseButtonPressed>()){
            if (box_.getGlobalBounds().contains(mouse_pos)) {
                is_focused_ = true;
                box_.setFillColor(kColorFocused);
                box_.setOutlineColor(sf::Color::Yellow);
            } else {
                is_focused_ = false;
                box_.setFillColor(kColorUnfocused);
                box_.setOutlineColor(sf::Color::White);
            }
        }

        if (is_focused_) {
            if (const auto* textEvent = event.getIf<sf::Event::TextEntered>()) {
                if (textEvent->unicode < 128) {
                    char entered = static_cast<char>(textEvent->unicode);
                    if (entered == '\b') {
                        if (!content_.empty()) {
                            content_.pop_back();
                        }
                    } else if (entered == '\r' || entered == '\n') {
                        is_focused_ = false;
                        ready_to_draw_ = true;
                        box_.setFillColor(kColorUnfocused);
                        box_.setOutlineColor(sf::Color::White);
                        
                    } else if (entered >= 32) {
                        content_ += entered;
                    }
                    display_text_.setString(content_);
                }
            }

        }
    }

    void TextBox::Draw(sf::RenderWindow& window) const {
        window.draw(box_);
        window.draw(display_text_);
    }

    std::string TextBox::GetText() const {
        return content_;
    }

    void TextBox::SetText(const std::string& text) {
        content_ = text;
        display_text_.setString(content_);
    }

    HarmonicScreen::HarmonicScreen(sf::Vector2f position, sf::Vector2f size, std::vector<std::function<float(float)>> harmonics, sf::Color color): 
            position_(position), size_(size), harmonics_(harmonics) {

        background_.setFillColor(color);
        background_.setSize(size);
        background_.setPosition(position);

        sf::Vertex line_start;
        line_start.position = {position.x, position.y + size.y/ 2.f};
        line_start.color = sf::Color::White;

        sf::Vertex line_end;
        line_end.position = {position.x + size.x, position.y + size.y / 2.f};
        line_end.color = sf::Color::White;

        line_vertices_ = {line_start, line_end};

        // Grid lines vertices 
        kTickSpacing_ = size.x / number_divisions_;
        for (int i = 0; i <= number_divisions_; ++i) {
            float current_x = position.x + (i * kTickSpacing_);
            float centerY = position.y + (size.y / 2.f);

            sf::Vertex tickUp;
            tickUp.position = {current_x, centerY - kTickSize_};
            tickUp.color = sf::Color::White;
            grid_lines_.push_back(tickUp);

            sf::Vertex tickDown;
            tickDown.position = {current_x, centerY + kTickSize_};
            tickDown.color = sf::Color::White;
            grid_lines_.push_back(tickDown); 

        }
    }

    std::vector<sf::Vertex> HarmonicScreen::CalculateFunctionVertices(std::function<float(float)> func) 
    {
        std::vector<sf::Vertex> vertices;
        
        float centerY = position_.y + (size_.y / 2.0f);
        float verticalScale = size_.y / 2.0f;

        for (float x_local = 0.0f; x_local <= size_.x; x_local += 1.0f) {
            float x_math = (x_local / size_.x) * math_range_x_;

            float y_math = func(x_math);
            
            float final_x = position_.x + x_local;
            float final_y = centerY - (y_math * verticalScale);

            sf::Vertex point;
            point.position = {final_x, final_y};
            point.color = sf::Color::Magenta;
            vertices.push_back(point);
        }

        return vertices;
    }

    void HarmonicScreen::Draw(sf::RenderWindow& window) const {
        float kWidth = 800;
        float kHeight = 800;

        window.draw(background_);

        sf::View vistaOriginal = window.getView();
        sf::View screen;

        // Size of the object
        screen.setSize({size_});

        // Center on the object
        screen.setCenter({position_.x + size_.x / 2.f, position_.y + size_.y / 2.f});

        // Position and Size (%)
        screen.setViewport(sf::FloatRect({position_.x / kWidth, (abs(position_.y) + size_.y) / kHeight}, {size_.x / kWidth, size_.y / kHeight}));

        window.setView(screen);

        window.draw(line_vertices_.data(), line_vertices_.size(), sf::PrimitiveType::Lines);
        window.draw(grid_lines_.data(), grid_lines_.size(), sf::PrimitiveType::Lines);

        if (!func_vertices_.empty()) {
            window.draw(func_vertices_.data(), func_vertices_.size(), sf::PrimitiveType::LineStrip);
        }

        window.setView(vistaOriginal);
    }

    void HarmonicScreen::UpdateHarmonicIndex(int index){
        if (index >= 0 && index < harmonics_.size()) {
            current_harmonic_index_ = index;
            auto target_func = harmonics_[current_harmonic_index_];
            func_vertices_ = CalculateFunctionVertices(target_func);
        }

    }

    void HarmonicScreen::SetHarmonics(const std::vector<std::function<float(float)>>& new_harmonics) {
        harmonics_ = new_harmonics;

        if (current_harmonic_index_ >= harmonics_.size()) {
            current_harmonic_index_ = static_cast<int>(harmonics_.size()) - 1;
        }

        if (!harmonics_.empty() && current_harmonic_index_ >= 0) {
            func_vertices_ = CalculateFunctionVertices(harmonics_[current_harmonic_index_]);
        } else {
            func_vertices_.clear();
        }
    }

} // namespace ui