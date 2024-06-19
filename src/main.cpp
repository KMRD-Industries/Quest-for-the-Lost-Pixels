#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>

#include "Coordinator.h"
#include "Game.h"
#include "InputHandler.h"
#include "Paths.h"
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
        else if (event.type == sf::Event::MouseMoved && config::debugMode)
        {
            sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
            std::cout << "Pozycja myszki: x=" << mousePosition.x << " y=" << mousePosition.y << std::endl;
        }
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }
    }
}

sf::Color hexStringToSfmlColor(const std::string& hexColor)
{
    std::string hex = (hexColor[0] == '#') ? hexColor.substr(1) : hexColor;

    std::istringstream iss(hex);
    int rgbValue = 0;
    iss >> std::hex >> rgbValue;

    int red = (rgbValue >> 16) & 0xFF;
    int green = (rgbValue >> 8) & 0xFF;
    int blue = rgbValue & 0xFF;

    return sf::Color(red, green, blue);
}

int main()
{
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Quest for the lost pixels!");

    window.create(desktopMode, "Quest for the lost pixels!", sf::Style::Default);

    int _ = ImGui::SFML::Init(window);
    window.setFramerateLimit(60);

    sf::Clock deltaClock;
    Game game;
    game.init();

    sf::Color customColor = hexStringToSfmlColor(config::backgroundColor);

    sf::Font font;
    if (!font.loadFromFile(std::string(ASSET_PATH) + "/fonts/Bentinck-Regular.ttf"))
    {
        std::cerr << "Failed to load font file!" << std::endl;
        return EXIT_FAILURE;
    }

    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(20);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10.f, 10.f);

    sf::Time currentTime;
    sf::Time lastTime = deltaClock.getElapsedTime();

    while (window.isOpen())
    {
        // Clear the window before drawing
        window.clear(customColor);

        game.update();
        game.draw();
        game.handleCollision();
        gCoordinator.getRegisterSystem<RenderSystem>()->draw(window);

        window.draw(fpsText);

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::SFML::Render(window);
        window.display();
        handleInput(window);

        currentTime = deltaClock.getElapsedTime();
        float fps = 1.f / (currentTime.asSeconds() - lastTime.asSeconds());
        lastTime = currentTime;

        fpsText.setString(std::to_string(fps));
    }
}
