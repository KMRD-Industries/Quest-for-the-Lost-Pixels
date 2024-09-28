#include "MainMenuState.h"


#include "Paths.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"

sf::Texture myTexture;

void MainMenuState::init()
{
    if (!myTexture.loadFromFile(std::string(ASSET_PATH) + "/ui/uiHigh.png"))
    {
        // Obs³uga b³êdu ³adowania tekstury
    }
}

void MainMenuState::render()
{
    sf::IntRect normalRect(0, 387, 144, 42);
    sf::IntRect hoverRect(144, 387, 144, 42);
    sf::IntRect clickedRect(288, 387, 144, 42);

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(0, 0));

    ImGui::Begin("ButtonWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
                 ImGuiWindowFlags_NoSavedSettings);

    ImTextureID myTextureID = reinterpret_cast<ImTextureID>(myTexture.getNativeHandle());
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0)); // Przezroczysty kolor
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0)); // Przezroczysty kolor podczas naje¿d¿ania
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0)); // Przezroczysty kolor podczas klikniêcia

    // Rysowanie przycisku z odpowiednimi wspó³rzêdnymi UV
    if (ImGui::ImageButton(myTextureID, ImVec2(normalRect.width, normalRect.height), m_uv0, m_uv1))
    {
        // Akcja po klikniêciu
    }
    ImGui::PopStyleColor(3); // Przywrócenie poprzedniego koloru

    // SprawdŸ, czy guzik jest najechany lub klikniêty
    if (ImGui::IsItemHovered())
    {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            // Klikniêty
            m_uv0 = ImVec2(static_cast<float>(clickedRect.left) / myTexture.getSize().x,
                           static_cast<float>(clickedRect.top) / myTexture.getSize().y);
            m_uv1 = ImVec2(static_cast<float>(clickedRect.left + clickedRect.width) / myTexture.getSize().x,
                           static_cast<float>(clickedRect.top + clickedRect.height) / myTexture.getSize().y);
        }
        else
        {
            // Najechany
            m_uv0 = ImVec2(static_cast<float>(hoverRect.left) / myTexture.getSize().x,
                           static_cast<float>(hoverRect.top) / myTexture.getSize().y);
            m_uv1 = ImVec2(static_cast<float>(hoverRect.left + hoverRect.width) / myTexture.getSize().x,
                           static_cast<float>(hoverRect.top + hoverRect.height) / myTexture.getSize().y);
        }
    }
    else
    {
        // Normalny stan
        m_uv0 = ImVec2(static_cast<float>(normalRect.left) / myTexture.getSize().x,
                       static_cast<float>(normalRect.top) / myTexture.getSize().y);
        m_uv1 = ImVec2(static_cast<float>(normalRect.left + normalRect.width) / myTexture.getSize().x,
                       static_cast<float>(normalRect.top + normalRect.height) / myTexture.getSize().y);
    }

    ImGui::End();
}

void MainMenuState::update(const float deltaTime)
{
    // Aktualizacja logiki, jeœli jest potrzebna
}