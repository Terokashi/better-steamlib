#pragma once

#include <string>

enum class Color {
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
    Color color = Color::Default;
    bool bold = false;
    bool dim = false;
};

inline Style headerStyle{ Color::Cyan, true, false, };

inline Style nameStyle { Color::White, false, false };

inline Style genreStyle { Color::Green, false, false };

inline Style pathStyle{ Color::Magenta, false, true };

inline Style sortStyle{ Color::Red, false, false };

inline int getColor(Color color)
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

inline std::string applyStyle(const std::string &text, const Style &style) {
    std::string styled_str = "\033[";
    bool first = true;

    if (style.bold) { styled_str += (first ? "" : ";") + std::to_string(1); first = false; }
    if (style.dim) { styled_str += (first ? "" : ";") + std::to_string(2); first = false; }
    styled_str += (first ? "" : ";") + std::to_string(getColor(style.color));
    styled_str += "m" + text + "\033[0m";

    return styled_str;
}