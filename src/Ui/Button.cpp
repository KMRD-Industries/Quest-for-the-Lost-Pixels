#include "Button.h"

#include "ButtonDTO.h"
#include "ResourceManager.h"
#include "TextTagComponent.h"

void Button::render()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::Begin("ButtonWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
                 ImGuiWindowFlags_NoSavedSettings);

    ImGui::SetCursorPos(m_position);

    ImTextureID atlasID = reinterpret_cast<ImTextureID>(m_loadedTextures.getNativeHandle());
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

    if (ImGui::ImageButton(
        atlasID, ImVec2(m_texture.width * m_scale, m_texture.height * m_scale),
        m_uv0, m_uv1))
    {
    }

    ImGui::PopStyleColor(3);

    const float buttonAtlasX = static_cast<float>(m_loadedTextures.getSize().x);
    const float buttonAtlasY = static_cast<float>(m_loadedTextures.getSize().y);
    if (ImGui::IsItemHovered())
    {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            const auto& clickedRect = m_clickedTexture;
            m_uv0 = ImVec2(static_cast<float>(clickedRect.left) / buttonAtlasX,
                           static_cast<float>(clickedRect.top) / buttonAtlasY);
            m_uv1 = ImVec2(
                static_cast<float>(clickedRect.left + clickedRect.width) / buttonAtlasX,
                static_cast<float>(clickedRect.top + clickedRect.height) / buttonAtlasY);
        }
        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            if (m_onClickedFunction)
                m_onClickedFunction();
        }
        else
        {
            const auto& hoverRect = m_hooveredTexture;
            m_uv0 = ImVec2(static_cast<float>(hoverRect.left) / buttonAtlasX,
                           static_cast<float>(hoverRect.top) / buttonAtlasY);
            m_uv1 = ImVec2(
                static_cast<float>(hoverRect.left + hoverRect.width) / buttonAtlasX,
                static_cast<float>(hoverRect.top + hoverRect.height) / buttonAtlasY);
        }
    }
    else
    {
        m_uv0 = ImVec2(static_cast<float>(m_texture.left) / buttonAtlasX,
                       static_cast<float>(m_texture.top) / buttonAtlasY);
        m_uv1 = ImVec2(static_cast<float>(m_texture.left + m_texture.width) / buttonAtlasX,
                       static_cast<float>(m_texture.top + m_texture.height) / buttonAtlasY);
    }

    ImGui::PushFont(m_loadedFonts);

    ImVec2 buttonSize = ImVec2(m_texture.width * m_scale,
                               m_texture.height * m_scale);
    ImVec2 textSize = ImGui::CalcTextSize(m_text.data());

    ImVec2 textPos = ImVec2(
        m_position.x + (buttonSize.x - textSize.x) * 0.5f,
        m_position.y + (buttonSize.y - textSize.y) * 0.5f
        );

    ImGui::SetCursorScreenPos(textPos);
    ImGui::Text(m_text.data());

    ImGui::PopFont();

    ImGui::End();
}

void Button::init(const ButtonDTO& config)
{
    m_atlasPath = config.m_atlasPath;
    m_fontPath = config.m_fontPath;
    m_text = config.m_text;
    m_textSize = config.m_textSize;
    m_scale = config.m_scale;
    m_position = config.m_position;
    m_texture = config.m_texture;
    m_hooveredTexture = config.m_hooveredTexture;
    m_clickedTexture = config.m_clickedTexture;
    m_uv0 = config.m_uv0;
    m_uv1 = config.m_uv1;
    m_onClickedFunction = config.m_onClickedFunction;

    ResourceManager& resourceManager = ResourceManager::getInstance();
    m_loadedTextures = resourceManager.getTexture(m_atlasPath);
    m_loadedFonts = resourceManager.getFont(m_fontPath, m_textSize);
}