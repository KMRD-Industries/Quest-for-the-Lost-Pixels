#include "Text.h"

#include "ResourceManager.h"
#include "TextDTO.h"
#include "TextTagComponent.h"

void Text::render()
{
    ImGui::Begin("ButtonWindow", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoSavedSettings);
    ImGui::SetCursorPos(m_position);

    ImGui::PushStyleColor(ImGuiCol_Text, m_color);

    ImGui::PushFont(m_loadedFonts);
    ImGui::TextUnformatted(m_text.c_str());
    ImGui::PopFont();

    ImGui::PopStyleColor();

    ImGui::End();
}

void Text::init(const TextDTO& config)
{
    m_fontPath = config.m_fontPath;
    m_text = config.m_text;
    m_textSize = config.m_textSize;
    m_scale = config.m_scale;
    m_position = config.m_position;
    m_color = config.m_color;

    ResourceManager& resourceManager = ResourceManager::getInstance();
    m_loadedFonts = resourceManager.getFont(m_fontPath, m_textSize);
}