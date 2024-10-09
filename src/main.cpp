#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <TextureSystem.h>
#include <imgui-SFML.h>
#include "Config.h"
#include "Coordinator.h"
#include "Game.h"
#include "InputHandler.h"
#include "MultiplayerSystem.h"
#include "RenderSystem.h"
#include "SpawnerSystem.h"
#include "TextTagSystem.h"

Coordinator gCoordinator;

void handleInput(sf::RenderWindow &window)
{
    sf::Event event{};
    InputHandler::getInstance()->update();

    while (window.pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(window, event);

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
        if (event.type == sf::Event::Closed) window.close();
    }
}

int main()
{
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(config::initWidth, config::initHeight), "Quest for the lost pixels!");

    window.create(desktopMode, "Quest for the lost pixels!", sf::Style::Default);

    int _ = ImGui::SFML::Init(window);
    window.setFramerateLimit(config::debugMode ? 144 : config::frameCycle);

    sf::Clock deltaClock;
    Game game;
    game.init();

    RenderSystem *m_renderSystem = gCoordinator.getRegisterSystem<RenderSystem>().get();
    TextTagSystem *m_textTagSystem = gCoordinator.getRegisterSystem<TextTagSystem>().get();
    TextureSystem *m_textureSystem = gCoordinator.getRegisterSystem<TextureSystem>().get();

    while (window.isOpen())
    {
        sf::Time deltaTime = deltaClock.restart();
        window.clear(m_textureSystem->getBackgroundColor());
        ImGui::SFML::Update(window, deltaTime);

        const auto deltaTimeFloat = static_cast<float>(deltaTime.asMilliseconds());

        game.update(deltaTimeFloat);
        game.handleCollision(deltaTimeFloat);
        game.draw();

        m_renderSystem->draw(window);
        m_textTagSystem->render(window);

        ImGui::SFML::Render(window);
        window.display();
        handleInput(window);
    }

    gCoordinator.getRegisterSystem<MultiplayerSystem>()->disconnect();
    ImGui::SFML::Shutdown();
    return 0;
}
