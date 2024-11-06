#pragma once
#include <imgui.h>
#include <string>

struct TextDTO;

class Text
{
public:
    void render();
    void init(const TextDTO& config);

private:
    std::string m_fontPath{};

    std::string m_text{};
    float m_textSize{};
    float m_scale{};
    ImVec4 m_color{};

    ImVec2 m_position{};

    ImFont* m_loadedFonts{};
};