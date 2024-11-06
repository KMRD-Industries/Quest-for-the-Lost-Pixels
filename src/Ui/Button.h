#pragma once
#include <functional>
#include <imgui.h>
#include <string>

#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/Texture.hpp"

struct ButtonDTO;

class Button
{
public:
    void render();
    void init(const ButtonDTO& config);

private:
    std::string m_atlasPath{};
    std::string m_fontPath{};

    std::string m_text{};
    float m_textSize{};
    float m_scale{};

    ImVec2 m_position{};

    sf::IntRect m_texture{};
    sf::IntRect m_hooveredTexture{};
    sf::IntRect m_clickedTexture{};

    ImVec2 m_uv0;
    ImVec2 m_uv1;

    std::function<void()> m_onClickedFunction;

    ImFont* m_loadedFonts{};
    sf::Texture m_loadedTextures{};
};