#include "ButtonSystem.h"

#include <imgui-SFML.h>

#include "ButtonComponent.h"
#include "Coordinator.h"
#include "SFML/Graphics/Texture.hpp"

extern Coordinator gCoordinator;

void ButtonSystem::render()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::Begin("ButtonWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
                 ImGuiWindowFlags_NoSavedSettings);


    for (const auto entity : m_entities)
    {
        if (m_entities.empty())
            return;
        auto& buttonComponent = gCoordinator.getComponent<ButtonComponent>(entity);

        ImGui::SetCursorPos(buttonComponent.position);

        auto& buttonAtlas = m_loadedTextures[buttonComponent.atlasPath];
        auto& buttonTexture = buttonComponent.texture;
        ImTextureID atlasID = reinterpret_cast<ImTextureID>(buttonAtlas.getNativeHandle());
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

        if (ImGui::ImageButton(
            atlasID, ImVec2(buttonTexture.width * buttonComponent.scale, buttonTexture.height * buttonComponent.scale),
            buttonComponent.m_uv0, buttonComponent.m_uv1))
        {
        }

        ImGui::PopStyleColor(3);

        const float buttonAtlasX = static_cast<float>(buttonAtlas.getSize().x);
        const float buttonAtlasY = static_cast<float>(buttonAtlas.getSize().y);
        if (ImGui::IsItemHovered())
        {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                const auto& clickedRect = buttonComponent.clickedTexture;
                buttonComponent.m_uv0 = ImVec2(static_cast<float>(clickedRect.left) / buttonAtlasX,
                                               static_cast<float>(clickedRect.top) / buttonAtlasY);
                buttonComponent.m_uv1 = ImVec2(
                    static_cast<float>(clickedRect.left + clickedRect.width) / buttonAtlasX,
                    static_cast<float>(clickedRect.top + clickedRect.height) / buttonAtlasY);
            }
            else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                if (buttonComponent.m_onClickedFunction)
                    buttonComponent.m_onClickedFunction();
            }
            else
            {
                const auto& hoverRect = buttonComponent.hooveredTexture;
                buttonComponent.m_uv0 = ImVec2(static_cast<float>(hoverRect.left) / buttonAtlasX,
                                               static_cast<float>(hoverRect.top) / buttonAtlasY);
                buttonComponent.m_uv1 = ImVec2(
                    static_cast<float>(hoverRect.left + hoverRect.width) / buttonAtlasX,
                    static_cast<float>(hoverRect.top + hoverRect.height) / buttonAtlasY);
            }
        }
        else
        {
            buttonComponent.m_uv0 = ImVec2(static_cast<float>(buttonTexture.left) / buttonAtlasX,
                                           static_cast<float>(buttonTexture.top) / buttonAtlasY);
            buttonComponent.m_uv1 = ImVec2(static_cast<float>(buttonTexture.left + buttonTexture.width) / buttonAtlasX,
                                           static_cast<float>(buttonTexture.top + buttonTexture.height) / buttonAtlasY);
        }

        auto buttonFont = m_loadedFonts[buttonComponent.fontPath + std::to_string(buttonComponent.textSize)];
        ImGui::PushFont(buttonFont);

        ImVec2 buttonSize = ImVec2(buttonTexture.width * buttonComponent.scale,
                                   buttonTexture.height * buttonComponent.scale);
        ImVec2 textSize = ImGui::CalcTextSize(buttonComponent.text.data());

        ImVec2 textPos = ImVec2(
            buttonComponent.position.x + (buttonSize.x - textSize.x) * 0.5f,
            buttonComponent.position.y + (buttonSize.y - textSize.y) * 0.5f
            );

        ImGui::SetCursorScreenPos(textPos);
        ImGui::Text(buttonComponent.text.data());

        ImGui::PopFont();
    }
    ImGui::End();
}

void ButtonSystem::loadData(const std::string& atlasPath, const std::string& fontPath, const float fontSize)
{
    if (!m_loadedTextures.contains(atlasPath))
        m_loadedTextures[atlasPath].loadFromFile(atlasPath);
    if (!m_loadedFonts.contains(fontPath + std::to_string(fontSize)))
    {
        ImGuiIO& io = ImGui::GetIO();
        ImFont* loadedFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize);
        m_loadedFonts[fontPath + std::to_string(fontSize)] = loadedFont;
        ImGui::SFML::UpdateFontTexture();
    }
}