#pragma once
#include <imgui.h>
#include <string>

#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/Texture.hpp"


struct InputTextDTO;

class InputText
{
public:
    void render();
    void init(const InputTextDTO& config);
    std::string getString() const;

private:
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

    ImVec2 m_uv0;
    ImVec2 m_uv1;

    ImFont* m_loadedFonts{};
    sf::Texture m_loadedTextures{};
};