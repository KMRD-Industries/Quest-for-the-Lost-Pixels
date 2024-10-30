#include "InputText.h"

#include "ButtonDTO.h"
#include "InputTextDTO.h"
#include "ResourceManager.h"

void InputText::render()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::Begin("ButtonWindow", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoSavedSettings);

    ImGui::SetCursorPos(m_position);

    ImTextureID atlasID = reinterpret_cast<ImTextureID>(m_loadedTextures.getNativeHandle());
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

    ImGui::Image(atlasID, ImVec2(m_texture.width * m_scale, m_texture.height * m_scale), m_uv0, m_uv1);

    ImGui::PopStyleColor(3);

    ImGui::PushFont(m_loadedFonts);

    ImVec2 buttonSize = ImVec2(m_texture.width * m_scale, m_texture.height * m_scale);
    ImVec2 textSize = ImGui::CalcTextSize(m_text.data());

    ImVec2 textPos =
        ImVec2(m_position.x + m_leftOffset, m_position.y + (buttonSize.y - textSize.y) * 0.5f - m_topOffset);

    ImGui::SetCursorScreenPos(textPos);

    ImGui::PushItemWidth(m_inputTextWidth);

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));

    ImGui::InputText("##input", m_text.data(), 21);

    ImGui::PopStyleColor(4);

    ImGui::PopFont();

    ImGui::End();
}

void InputText::init(const InputTextDTO& config)
{
    m_atlasPath = config.m_atlasPath;
    m_fontPath = config.m_fontPath;
    m_text = config.m_text;
    m_textSize = config.m_textSize;
    m_scale = config.m_scale;
    m_position = config.m_position;
    m_texture = config.m_texture;
    m_leftOffset = config.m_leftOffset;
    m_topOffset = config.m_topOffset;
    m_inputTextWidth = config.m_inputTextWidth;

    ResourceManager& resourceManager = ResourceManager::getInstance();
    m_loadedTextures = resourceManager.getTexture(m_atlasPath);
    m_loadedFonts = resourceManager.getFont(m_fontPath, m_textSize);

    const float buttonAtlasX = static_cast<float>(m_loadedTextures.getSize().x);
    const float buttonAtlasY = static_cast<float>(m_loadedTextures.getSize().y);

    m_uv0 = ImVec2(static_cast<float>(m_texture.left) / buttonAtlasX, static_cast<float>(m_texture.top) / buttonAtlasY);
    m_uv1 = ImVec2(static_cast<float>(m_texture.left + m_texture.width) / buttonAtlasX,
                   static_cast<float>(m_texture.top + m_texture.height) / buttonAtlasY);
}

std::string InputText::getString() const { return m_text; }