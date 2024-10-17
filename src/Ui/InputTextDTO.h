#pragma once
#include <imgui.h>
#include <string>

#include "SFML/Graphics/Rect.hpp"

struct InputTextDTO
{
    std::string m_atlasPath{};
    std::string m_fontPath{};

    std::string m_text{};
    float m_textSize{};
    float m_scale{};

    int m_leftOffset{};
    int m_topOffset{};
    int m_inputTextWidth{};

    ImVec2 m_position{};

    sf::IntRect m_texture{};
};