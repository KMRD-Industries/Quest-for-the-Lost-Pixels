#pragma once
#include <imgui.h>
#include <string>

struct TextDTO
{
    std::string m_fontPath{};

    std::string m_text{};
    float m_textSize{};
    float m_scale{};
    ImVec4 m_color{1.f, 1.f, 1.f, 1.f};

    ImVec2 m_position{};
};