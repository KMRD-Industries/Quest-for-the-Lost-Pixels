#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>

#include "Coordinator.h"
#include "Game.h"
#include "InputHandler.h"
#include "MapSystem.h"
#include "RenderSystem.h"

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
        else if (event.type == sf::Event::KeyReleased)
        {
            const auto keyCode = event.key.code;
            InputHandler::getInstance()->handleKeyboardInput(keyCode, false);
        }
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(16 * 26 * 3, 720), "ImGui + SFML = <3");

    int _ = ImGui::SFML::Init(window);
    window.setFramerateLimit(60);

    sf::Clock deltaClock;

    Game game;
    game.init();

    while (window.isOpen())
    {
        game.update();

        ImGui::SFML::Update(window, deltaClock.restart());
        // Clear the window before drawing
        window.clear();

        gCoordinator.getRegisterSystem<RenderSystem>()->draw(window);
        game.draw();

        // Render ImGui
        ImGui::SFML::Render(window);
        // Display the rendered frame
        window.display();
        handleInput(window);
    }
}
