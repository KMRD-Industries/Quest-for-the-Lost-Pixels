#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "BackgroundSystem.h"
#include "Config.h"
#include "Coordinator.h"
#include "Game.h"
#include "InputHandler.h"
#include "MultiplayerSystem.h"
#include "RenderSystem.h"
#include "SpawnerSystem.h"
#include "TextTagSystem.h"

Coordinator gCoordinator;

void handleInput(sf::RenderWindow& window)
{
    sf::Event event{};
    InputHandler::getInstance()->update();

    while (window.pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(event);

        if (event.type == sf::Event::KeyPressed)
        {
            const auto keyCode = event.key.code;
            InputHandler::getInstance()->handleKeyboardInput(keyCode, true);
        }
        else if (event.type == sf::Event::MouseButtonPressed)
        {
            const auto keyCode = event.mouseButton.button;
            InputHandler::getInstance()->handleKeyboardInput(keyCode, true);
        }
        else if (event.type == sf::Event::KeyReleased)
        {
            const auto keyCode = event.key.code;
            InputHandler::getInstance()->handleKeyboardInput(keyCode, false);
        }
        else if (event.type == sf::Event::MouseButtonReleased)
        {
            const auto keyCode = event.mouseButton.button;
            InputHandler::getInstance()->handleKeyboardInput(keyCode, false);
        }
        else if (event.type == sf::Event::MouseMoved)
        {
            const auto mousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
            InputHandler::getInstance()->updateMousePosition(mousePosition);
        }
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

sf::Color hexStringToSfmlColor(const std::string& hexColor)
{
    const std::string hex = hexColor[0] == '#' ? hexColor.substr(1) : hexColor;

    std::istringstream iss(hex);
    int rgbValue = 0;
    iss >> std::hex >> rgbValue;

    const uint8 red = rgbValue >> 16 & 0xFF;
    const uint8 green = rgbValue >> 8 & 0xFF;
    const uint8 blue = rgbValue & 0xFF;

    return {red, green, blue};
}

int main()
{
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(config::initWidth, config::initHeight), "Quest for the lost pixels!");

    // W³aœciwa inicjalizacja okna
    window.create(desktopMode, "Quest for the lost pixels!", sf::Style::Fullscreen);

    // Inicjalizacja ImGui
    int _ = ImGui::SFML::Init(window);
    window.setFramerateLimit(config::frameCycle);
    ImGui::CreateContext();

    sf::Clock deltaClock;
    Game game;
    game.init();

    sf::Color customColor = hexStringToSfmlColor(config::backgroundColor);

    while (window.isOpen())
    {
        sf::Time deltaTime = deltaClock.restart();

        // Clear the window before drawing
        window.clear(customColor);

        // Upewnij siê, ¿e ImGui wchodzi w now¹ ramkê
        ImGui::SFML::Update(window, deltaTime);

        // Logika gry
        game.handleCollision();
        game.update(deltaTime.asSeconds());
        game.draw(window);

        // Rysowanie systemów
        gCoordinator.getRegisterSystem<RenderSystem>()->draw(window);
        gCoordinator.getRegisterSystem<BackgroundSystem>()->draw(window);
        gCoordinator.getRegisterSystem<TextTagSystem>()->render(window);

        // Renderowanie ImGui
        ImGui::SFML::Render(window); // Renderowanie UI SFML

        // Wyœwietlenie okna
        window.display();

        // Obs³uga wejœcia
        handleInput(window);
    }

    // Zakoñczenie po³¹czenia i zamkniêcie ImGui
    gCoordinator.getRegisterSystem<MultiplayerSystem>()->disconnect();
    ImGui::SFML::Shutdown();
    return 0;
}