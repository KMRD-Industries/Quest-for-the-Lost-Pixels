#include "MainMenuState.h"

#include <imgui-SFML.h>
#include <iostream>
#include "Paths.h"
#include "SFML/Graphics/Font.hpp"  // Dodaj t� lini�
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"

sf::Texture myTexture;
sf::Font myFont; // Zmienna do przechowywania czcionki
ImFont* myImGuiFont;

void MainMenuState::init()
{
    if (!myTexture.loadFromFile(std::string(ASSET_PATH) + "/ui/uiHigh.png"))
        std::cerr << "B��d �adowania tekstury." << std::endl;

    // Za�aduj czcionk�
    if (!myFont.loadFromFile(std::string(ASSET_PATH) + "/ui/uiFont.ttf")) // Zmiana �cie�ki
        std::cerr << "B��d �adowania czcionki." << std::endl;


    ImGuiIO& io = ImGui::GetIO();
    myImGuiFont = io.Fonts->AddFontFromFileTTF((ASSET_PATH + std::string("/ui/uiFont.ttf")).c_str(), 30.0f);
    if (myImGuiFont == nullptr)
        std::cerr << "B��d �adowania czcionki ImGui." << std::endl;
    ImGui::SFML::UpdateFontTexture(); // important call: updates font texture


    m_uv0 = ImVec2(0, 0);
    m_uv1 = ImVec2(1, 1);
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
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0)); // Przezroczysty kolor podczas naje�d�ania
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0)); // Przezroczysty kolor podczas klikni�cia

    // Rysowanie przycisku z odpowiednimi wsp�rz�dnymi UV
    if (ImGui::ImageButton(myTextureID, ImVec2(normalRect.width, normalRect.height), m_uv0, m_uv1))
    {
        // Akcja po klikni�ciu
    }

    ImGui::PopStyleColor(3); // Przywr�cenie poprzedniego koloru

    // Sprawd�, czy guzik jest najechany lub klikni�ty
    if (ImGui::IsItemHovered())
    {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            // Klikni�ty
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

    ImGui::PushFont(myImGuiFont);
    const char* buttonText = "Click Me";
    ImVec2 buttonSize = ImVec2(normalRect.width, normalRect.height);
    ImVec2 textSize = ImGui::CalcTextSize(buttonText);

    // Oblicz pozycj� tekstu
    ImVec2 posOfButton = ImGui::GetItemRectMin();
    ImVec2 offset = {(buttonSize.x - textSize.x) * 0.5f,
                     (buttonSize.y - textSize.y) * 0.5f};
    ImVec2 textPos = {posOfButton.x + offset.x, posOfButton.y + offset.y};

    // Ustaw pozycj� kursora na �rodku przycisku
    ImGui::SetCursorScreenPos(textPos);

    // U�yj wi�kszego fontu, je�li jest zdefiniowany
    ImGui::Text("%s", buttonText);
    ImGui::PopFont();


    ImGui::End();
}

void MainMenuState::update(const float deltaTime)
{
    // Aktualizacja logiki, je�li jest potrzebna
}