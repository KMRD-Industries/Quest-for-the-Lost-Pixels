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

Coordinator gCoordinator;
PublicConfigSingleton configSingleton;

void handleInput(sf::RenderWindow& window, const sf::RenderTexture& texture)
{
    sf::Event event{};
    const auto inputHandler = InputHandler::getInstance();
    inputHandler->update();

    while (window.pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(event);

        if (event.type == sf::Event::KeyPressed)
        {
            const auto keyCode = event.key.code;
            inputHandler->handleKeyboardInput(keyCode, true);
        }
        else if (event.type == sf::Event::MouseButtonPressed)
        {
            const auto keyCode = event.mouseButton.button;
            inputHandler->handleKeyboardInput(keyCode, true);
        }
        else if (event.type == sf::Event::KeyReleased)
        {
            const auto keyCode = event.key.code;
            inputHandler->handleKeyboardInput(keyCode, false);
        }
        else if (event.type == sf::Event::MouseButtonReleased)
        {
            const auto keyCode = event.mouseButton.button;
            inputHandler->handleKeyboardInput(keyCode, false);
        }
        else if (event.type == sf::Event::MouseMoved)
        {
            // Capture mouse position from window with view
            const auto worldPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), texture.getView());
            InputHandler::getInstance()->updateMousePosition(worldPosition);
        }
        else if (event.type == sf::Event::Resized)
        {
            inputHandler->updateWindowSize({event.size.width, event.size.height});
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
        handleInput(window, renderTexture);
    }

    gCoordinator.getRegisterSystem<MultiplayerSystem>()->disconnect();
    ImGui::SFML::Shutdown();
    return 0;
}