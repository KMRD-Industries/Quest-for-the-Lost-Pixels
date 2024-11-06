#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <TextureSystem.h>
#include <imgui-SFML.h>

#include "Coordinator.h"
#include "Game.h"
#include "InputHandler.h"
#include "MultiplayerSystem.h"
#include "Paths.h"
#include "PublicConfigMenager.h"
#include "ResourceManager.h"
#include "SoundManager.h"
#include "SpawnerSystem.h"
#include "Timer.h"

Coordinator gCoordinator;
PublicConfigSingleton configSingleton;

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
        if (event.type == sf::Event::Closed) window.close();
    }
}

int main()
{
    configSingleton.LoadConfig(ASSET_PATH + std::string("/config.json"));
    const PublicConfig& config = configSingleton.GetConfig();

    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktopMode, "Quest for the lost pixels!");
    Timer* timer = Timer::Instance();

    window.create(desktopMode, "Quest for the lost pixels!", sf::Style::Default);

    int _ = ImGui::SFML::Init(window);
    window.setFramerateLimit(config.debugMode ? 144 : config.frameCycle);
    ImGui::CreateContext();

    sf::Clock deltaClock;
    Game game;
    game.init();

    ResourceManager& resourceManager = ResourceManager::getInstance();
    resourceManager.getFont(ASSET_PATH + std::string("/ui/uiFont.ttf"), 160);
    resourceManager.getFont(ASSET_PATH + std::string("/ui/uiFont.ttf"), 40);
    resourceManager.loadShader(ASSET_PATH + std::string("/shaders/grayscale.frag"), video::FragmentShader::DEATH);

    SoundManager& soundManager = SoundManager::getInstance();
    soundManager.loadSound(Sound::Type::MenuBackgroundMusic,
                           ASSET_PATH + std::string("/sounds/menuBackgroundSound.mp3"));
    soundManager.loadSound(Sound::Type::GameBackgroundMusic,
                           ASSET_PATH + std::string("/sounds/gameBackgroundSound.mp3"));

    sf::RenderTexture renderTexture;
    if (!renderTexture.create(window.getSize().x, window.getSize().y))
    {
    }
    while (window.isOpen())
    {
        sf::Time deltaTime = deltaClock.restart();
        renderTexture.clear(gCoordinator.getRegisterSystem<TextureSystem>()->getBackgroundColor());

        ImGui::SFML::Update(window, deltaTime);
        timer->Tick();
        timer->Reset();

        game.update(static_cast<float>(deltaTime.asMilliseconds()));
        game.draw(renderTexture);

        ImGui::SFML::Render(renderTexture);
        renderTexture.display();

        window.clear();
        sf::Sprite sprite(renderTexture.getTexture());
        if (resourceManager.getCurretShaderType() == video::FragmentShader::NONE)
            window.draw(sprite);
        else
            window.draw(sprite, resourceManager.getCurrentShader().get());


        window.display();
        handleInput(window);
    }

    gCoordinator.getRegisterSystem<MultiplayerSystem>()->disconnect();
    ImGui::SFML::Shutdown();
    return 0;
}