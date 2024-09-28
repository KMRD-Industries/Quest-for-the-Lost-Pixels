#include "ButtonSystem.h"

#include "ButtonComponent.h"
#include "Coordinator.h"
#include "SFML/Graphics/Texture.hpp"

extern Coordinator gCoordinator;

void ButtonSystem::render()
{
    for (const auto entity : m_entities)
    {
        auto& buttonComponent = gCoordinator.getComponent<ButtonComponent>(entity);

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(0, 0));

        ImGui::Begin("ButtonWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoSavedSettings);

        ImGui::SetCursorPos(buttonComponent.position);

        auto& buttonAtlas = m_loadedTextures[buttonComponent.atlasPath];
        auto& buttonTexture = buttonComponent.texture;
        ImTextureID atlasID = reinterpret_cast<ImTextureID>(buttonAtlas.getNativeHandle());
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

        if (ImGui::ImageButton(
            atlasID, ImVec2(static_cast<float>(buttonTexture.width), static_cast<float>(buttonTexture.height)),
            buttonComponent.m_uv0, buttonComponent.m_uv1))
        {
            if (buttonComponent.m_onClickedFunction)
                buttonComponent.m_onClickedFunction();
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
        ImVec2 posOfButton = ImGui::GetItemRectMin();
        ImVec2 textSize = ImGui::CalcTextSize(buttonComponent.text.data());
        ImVec2 textPos = ImVec2(
            posOfButton.x + (buttonTexture.width - textSize.x) * 0.5f,
            posOfButton.y + (buttonTexture.height - textSize.y) * 0.5f
            );

        ImGui::SetCursorScreenPos(textPos);

        ImGui::Text(buttonComponent.text.data());
        ImGui::PopFont();

        ImGui::End();
    }
}