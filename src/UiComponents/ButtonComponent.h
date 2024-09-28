#pragma once
#include <functional>
#include <imgui.h>
#include <string>

#include "SFML/Graphics/Rect.hpp"

struct ButtonComponent
{
    std::string atlasPath{};
    std::string fontPath{};

    std::string text{};
    float textSize{};

    ImVec2 position{};

    sf::IntRect texture{};
    sf::IntRect hooveredTexture{};
    sf::IntRect clickedTexture{};

    ImVec2 m_uv0;
    ImVec2 m_uv1;

    std::function<void()> m_onClickedFunction;
};