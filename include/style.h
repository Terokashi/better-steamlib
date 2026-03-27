#pragma once
#include <string>

enum Color {
    Black,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    Default
};

struct Style {
    Color color;
    bool bold;
    bool dim;
};

Style headerStyle{
    Color::Cyan,
    true,
    false,
};

Style nameStyle {
    Color::White,
    false,
    false
};

Style genreStyle {
    Color::Green,
    false,
    false
};

Style pathStyle{
    Color::Magenta,
    false,
    true
};

int getColor(Color color)
{
    switch(color) {
    case Color::Black:      return 30;
    case Color::Red:        return 31;
    case Color::Green:      return 32;
    case Color::Yellow:     return 33;
    case Color::Blue:       return 34;
    case Color::Magenta:    return 35;
    case Color::Cyan:       return 36;
    case Color::White:      return 37;
    case Color::Default:    return 39;
    default:                return 39;
    }
}

std::string applyStyle(const std::string &text, const Style &style)
{
    std::string styled_str;
    const std::string reset = "\033[0m";

    styled_str = "\033[" + std::to_string(getColor(style.color));
    if (style.dim)
        styled_str += ";2";
    if (style.bold)
        styled_str += ";1";
    styled_str += "m" + text + reset;

    return styled_str;
}