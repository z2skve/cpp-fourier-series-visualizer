#include <SFML/Graphics.hpp>
#include <cmath>
#include "function_generator.h"
#include "ui_elements.h"
#include "fourier_generator.h"
#include "math_engine.h"
#include <algorithm>

std::string round_to_string(float value, int n = 2) {
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << value;
    return out.str();
}

int main() {
    // Default function
    std::function<float(float)> target_func = [](float x) -> float {
        return std::sin(x*x) + x/10.f;
    };

    // Math parser setup
    ui::MathParser engine;

    // Window setup
    const float kWidth = 800;
    const float kHeight = 800;
    const float kPanelHeight = 200;

    const float kTickSpacing = 50.f;
    const float kTickSize = 3.f;

    const float kPixelsPerUnit = kTickSpacing;

    // Options panel position
    const float kOptionsPanelHeight = -kPanelHeight / 2.0f - kHeight / 2.0f;

    // Slider constants
    const float kSliderXOffset = 25.0f;
    const sf::Vector2f kHarmonicsSliderPosition = {kSliderXOffset, kOptionsPanelHeight + 50.0f};
    const float kSliderWidth = kWidth / 2.0f - 50.f;

    // Slider value ranges
    float slider_min_val = 0.0f;
    float slider_max_val = 500.0f;

    // Control variable to optimize redraws
    bool draw_last_loop = false;


    // Range for Fourier series
    float range_start = 0.0f;
    float range_end = 16.0f;

    // Load font
    sf::Font main_font;
    if (!main_font.openFromFile("LatinmodernmathRegular-z8EBa.otf")) {
        return -1; 
    }

    // Fourier generator instance
    fourier_sim::Generator fourier_sim;

    // Grid and labels setup
    sf::Text label_text(main_font);
    label_text.setCharacterSize(15);
    label_text.setFillColor(sf::Color::White);
    label_text.setScale({1.0f, -1.0f});

    sf::Text harmonics_value(main_font); 
    ui::setupText(harmonics_value, 14, {kSliderXOffset + 10.f, kOptionsPanelHeight + 80.0f});

    sf::Text slices_value(main_font);
    ui::setupText(slices_value, 14, {kSliderXOffset + 10.f, kOptionsPanelHeight + 130.0f});

    sf::Text max_value_text(main_font);
    ui::setupText(max_value_text, 14, {kWidth - kSliderXOffset - 140.f, kOptionsPanelHeight + 175.0f});
    max_value_text.setString("Max Value: ");

    sf::Text range_start_text(main_font);
    ui::setupText(range_start_text, 14, {kWidth - kSliderXOffset - 140.f, kOptionsPanelHeight + 125.0f});
    range_start_text.setString("Range Start: ");

    sf::Text range_end_text(main_font);
    ui::setupText(range_end_text, 14, {kWidth - kSliderXOffset - 140.f, kOptionsPanelHeight + 75.0f});
    range_end_text.setString("Range End: ");

    sf::Text user_info_text(main_font);
    ui::setupText(user_info_text, 13, {kWidth - 160.f, kOptionsPanelHeight + 20.f});
    user_info_text.setString("Math engine used: ExprTk");

    sf::View view;
    view.setSize({static_cast<float>(kWidth), -static_cast<float>(kHeight)});
    view.setCenter({kWidth / 2.f, -kPanelHeight / 2.f});

    sf::RenderWindow window(sf::VideoMode({(int)kWidth, (int)kHeight}), "Fourier Simulation", sf::Style::Titlebar | sf::Style::Close);
    window.setView(view); 

    // X axis line vertices
    sf::Vertex line_start;
    line_start.position = {0.f, 0.f};
    line_start.color = sf::Color::White;

    sf::Vertex line_end;
    line_end.position = {static_cast<float>(kWidth), 0.f};
    line_end.color = sf::Color::White;

    std::array<sf::Vertex, 2> line_vertices = {line_start, line_end};

    // Grid lines vertices
    std::vector<sf::Vertex> grid_lines;
    for (float x = 0.f; x<=kWidth; x += kTickSpacing){
        sf::Vertex tickUp;
        tickUp.position = {x, kTickSize};
        tickUp.color = sf::Color::White;
        grid_lines.push_back(tickUp);

        sf::Vertex tickDown;
        tickDown.position = {x, -kTickSize};
        tickDown.color = sf::Color::White;
        grid_lines.push_back(tickDown);

    }

    // Create sliders and panels
    ui::Panel options_panel({0, kOptionsPanelHeight}, {kWidth, kPanelHeight});
    ui::Panel division_panel({kWidth/2, kOptionsPanelHeight}, {5, kPanelHeight}, sf::Color(220, 220, 220));

    ui::Slider slices_slider({kSliderXOffset, kOptionsPanelHeight + 100.0f}, kSliderWidth, slider_min_val, slider_max_val);
    ui::Panel slices_panel({kSliderXOffset - 8.0f, kOptionsPanelHeight + 90.0f}, {kSliderWidth + 16.0f, 20.0f}, sf::Color(10, 10, 10));

    ui::Slider harmonics_slider(kHarmonicsSliderPosition, kSliderWidth, slider_min_val, slider_max_val);
    ui::Panel harmonics_panel({kSliderXOffset - 8.0f, kOptionsPanelHeight + 40.0f}, {kSliderWidth + 16.0f, 20.0f}, sf::Color(10, 10, 10));

    ui::TextBox function_input_box({kSliderXOffset, kOptionsPanelHeight + 150.f}, {kSliderWidth, 30.f}, 18, main_font);
    ui::TextBox max_value_input_box({kWidth - kSliderXOffset - 50.f, kOptionsPanelHeight + 150.f}, {50.f, 30.f}, 15, main_font);
    ui::TextBox range_start_input_box({kWidth - kSliderXOffset - 50.f, kOptionsPanelHeight + 100.f}, {50.f, 30.f}, 15, main_font);
    ui::TextBox range_end_input_box({kWidth - kSliderXOffset - 50.f, kOptionsPanelHeight + 50.f}, {50.f, 30.f}, 15, main_font);

    ui::HarmonicScreen harmonic_screen({kWidth / 2.f + 20.0f, -kHeight + 325.f}, {kWidth / 4.0f, 150.0f}, fourier_sim.GetHarmonics(), sf::Color::Black);

    function_input_box.SetText("sin(x*x) + x/10");
    max_value_input_box.SetText(round_to_string(slider_max_val, 0));
    range_start_input_box.SetText(round_to_string(range_start, 2));
    range_end_input_box.SetText(round_to_string(range_end, 2));

    // Initial function setup
    std::vector<sf::Vertex> equation_points = {};
    std::vector<sf::Vertex> fourier_points = {};

    float last_harmonics = -1.f;
    float last_slices = -1.f;
    while (window.isOpen()){
        while(const std::optional event = window.pollEvent()){
            if (event->is<sf::Event::Closed>()){
                window.close();
            }

            harmonics_slider.HandleEvent(*event, window);
            slices_slider.HandleEvent(*event, window);
            function_input_box.HandleEvent(*event, window);
            max_value_input_box.HandleEvent(*event, window);
            range_start_input_box.HandleEvent(*event, window);
            range_end_input_box.HandleEvent(*event, window);
        }

        float harmonics = harmonics_slider.GetValue();
        float slices = slices_slider.GetValue();

        bool has_changes = (last_harmonics != harmonics || 
                            last_slices != slices || 
                            function_input_box.IsFocused() || 
                            max_value_input_box.IsFocused() ||
                            range_start_input_box.IsFocused() || 
                            range_end_input_box.IsFocused()
                        );

        if (!has_changes && !draw_last_loop) {
            continue;
        }

        draw_last_loop = has_changes;

        last_harmonics = harmonics;
        last_slices = slices;

        if (has_changes) {
            fourier_points = fourier_sim.GetUniversalFourier(harmonics, slices, target_func, range_start, range_end);
            harmonic_screen.SetHarmonics(fourier_sim.GetHarmonics());
            harmonic_screen.UpdateHarmonicIndex(static_cast<int>(harmonics));
        }

        // Check if new function input is ready
        if (function_input_box.IsReadyToDraw()) {
            std::string func_text = function_input_box.GetText();

            // Parse func_text to create a new target function
            engine.Compile(func_text);
            target_func = engine.GetTargetFunction();

            // Reset sliders
            slices_slider.ResetValue();
            harmonics_slider.ResetValue();

            function_input_box.ResetReadyToDraw();
        }

        if (max_value_input_box.IsReadyToDraw()) {
            std::string max_value_text = max_value_input_box.GetText();
            if (max_value_text.empty()) {
                max_value_text = round_to_string(slider_max_val, 2);
            }

            try {
                float max_value = std::stof(max_value_text);
                slider_max_val = std::clamp(max_value, 0.0f, 10000.0f);                
                harmonics_slider.SetMaxValue(slider_max_val);
                slices_slider.SetMaxValue(slider_max_val);
            } catch (const std::invalid_argument&) {}
            max_value_input_box.SetText(round_to_string(slider_max_val, 0));
            max_value_input_box.ResetReadyToDraw();
        }

        if (range_start_input_box.IsReadyToDraw()) {
            std::string range_start_text = range_start_input_box.GetText();
            if (range_start_text.empty()) {
                range_start_text = std::to_string(range_start);
            }

            try {
                float user_range_start = std::stof(range_start_text);
                range_start = std::clamp(user_range_start, 0.0f, range_end - 0.1f);                
            } catch (const std::invalid_argument&) {}
            range_start_input_box.SetText(round_to_string(range_start, 2));

            // Just to force redraw
            last_harmonics = -1.0f;

            range_start_input_box.ResetReadyToDraw();
        }

        if (range_end_input_box.IsReadyToDraw()) {
            std::string range_end_text = range_end_input_box.GetText();
            if (range_end_text.empty()) {
                range_end_text = std::to_string(range_end);
            }

            try {
                float user_range_end = std::stof(range_end_text);
                range_end = std::clamp(user_range_end, range_start + 0.1f, 16.0f);
            } catch (const std::invalid_argument&) {}
            range_end_input_box.SetText(round_to_string(range_end, 2));

            // Just to force redraw
            last_harmonics = -1.0f;

            range_end_input_box.ResetReadyToDraw();
        }
        
        // Create objective function points
        equation_points = eq_sim::getInstance(target_func);

        // Clear window
        window.clear(sf::Color::Black);

        // Here drawing starts 

        window.draw(line_vertices.data(), line_vertices.size(), sf::PrimitiveType::Lines);
        window.draw(grid_lines.data(), grid_lines.size(), sf::PrimitiveType::Lines);

        // Draw labels
        for (float x = 0.f; x<=kWidth; x += kTickSpacing){
            label_text.setString(std::to_string(static_cast<int>(x / 50.f)));
            sf::FloatRect bounds = label_text.getLocalBounds();
            label_text.setOrigin({bounds.size.x / 2.0f, 0.0f});
            label_text.setPosition({x, -20.f});
            window.draw(label_text);
        }

        // Update and draw slider value
        harmonics_value.setString("Harmonics: " + std::to_string(static_cast<int>(harmonics)));
        slices_value.setString("Slices: " + std::to_string(static_cast<int>(slices)));


        sf::View vistaOriginal = window.getView();
        sf::View screen;

        screen.setSize({kWidth, -(kHeight - kPanelHeight)});
        screen.setCenter({kWidth / 2.f, 0.f});

        // Taking into account position of y axis
        screen.setViewport(sf::FloatRect({0.f, 0.f}, {kWidth / kWidth, (kHeight - kPanelHeight) / kHeight}));


        // Draw function points
        window.setView(screen);
        if (!equation_points.empty()) {
            window.draw(equation_points.data(), equation_points.size(), sf::PrimitiveType::LineStrip);
        }

        if (!fourier_points.empty()) {
            window.draw(fourier_points.data(), fourier_points.size(), sf::PrimitiveType::LineStrip);
        }

        window.setView(vistaOriginal);

        // Draw options panel
        options_panel.Draw(window);
        division_panel.Draw(window);

        // Draw Screen with harmonics
        harmonic_screen.Draw(window);

        // Draw slider panel
        slices_panel.Draw(window);
        harmonics_panel.Draw(window);

        // Draw sliders
        harmonics_slider.Draw(window);
        slices_slider.Draw(window);

        // Draw texts
        window.draw(harmonics_value);
        window.draw(slices_value);
        window.draw(max_value_text);
        window.draw(range_start_text);
        window.draw(range_end_text);
        window.draw(user_info_text);

        // Draw text boxes
        function_input_box.Draw(window);
        max_value_input_box.Draw(window);
        range_start_input_box.Draw(window);
        range_end_input_box.Draw(window);

        // Here drawing ends
        window.display();
    }

    return 0;
}
