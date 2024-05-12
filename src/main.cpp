#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include "Coordinator.h"
#include "MapComponent.h"
#include "MapSystem.h"
#include "RenderComponent.h"

#include "AnimationComponent.h"
#include "Paths.h"
#include "PlayerComponent.h"
#include "PlayerMovementSystem.h"
#include "PositionComponent.h"
#include "RenderSystem.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

Coordinator gCoordinator;

int main()
{
    gCoordinator.init();
    gCoordinator.registerComponent<AnimationFrame>();
    gCoordinator.registerComponent<MapComponent>();
    gCoordinator.registerComponent<PlayerComponent>();
    gCoordinator.registerComponent<PositionComponent>();
    gCoordinator.registerComponent<RenderComponent>();
    gCoordinator.registerComponent<TileComponent>();
    gCoordinator.registerComponent<TransformComponent>();
    gCoordinator.registerComponent<AnimationComponent>();

    auto renderSystem = gCoordinator.getRegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<RenderComponent>());
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        gCoordinator.setSystemSignature<RenderSystem>(signature);
    }

    auto playerMovementSystem = gCoordinator.getRegisterSystem<PlayerMovementSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        signature.set(gCoordinator.getComponentType<PlayerComponent>());
        gCoordinator.setSystemSignature<PlayerMovementSystem>(signature);
    }

    auto mapSystem = gCoordinator.getRegisterSystem<MapSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        signature.set(gCoordinator.getComponentType<TileComponent>());
        signature.set(gCoordinator.getComponentType<AnimationComponent>());
        gCoordinator.setSystemSignature<MapSystem>(signature);
    }

    auto textureSystem = gCoordinator.getRegisterSystem<TextureSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<RenderComponent>());
        signature.set(gCoordinator.getComponentType<AnimationComponent>());
        signature.set(gCoordinator.getComponentType<TileComponent>());
        gCoordinator.setSystemSignature<TextureSystem>(signature);
    }

    std::vector<Entity> entities(MAX_ENTITIES - 1);

    // Local player
    entities[0] = gCoordinator.createEntity();
    entities[1] = gCoordinator.createEntity();
    sf::Texture texture;
    std::string PathToAssets{ASSET_PATH};
    texture.loadFromFile(PathToAssets + "/knight/knight.png");

    gCoordinator.addComponent(entities[0], RenderComponent{.sprite = sf::Sprite(texture), .layer = 4});
    gCoordinator.addComponent(entities[0], TransformComponent(sf::Vector2f(0.f, 0.f), 0.f, sf::Vector2f(1.f, 1.f)));
    gCoordinator.addComponent(entities[0], AnimationComponent{});
    gCoordinator.addComponent(entities[0], PlayerComponent{});

    gCoordinator.addComponent(entities[1], RenderComponent{.sprite = sf::Sprite(texture)});
    gCoordinator.addComponent(entities[1], TransformComponent(sf::Vector2f(0.f, 0.f), 0.f, sf::Vector2f(1.f, 1.f)));
    sf::RenderWindow window(sf::VideoMode(16 * 26 * 3, 720), "ImGui + SFML = <3");

    textureSystem->loadFromFile(std::string(ASSET_PATH) + "/tileSets/CosmicLilacTiles.json");
    textureSystem->loadFromFile(std::string(ASSET_PATH) + "/tileSets/Decorative.json");
    textureSystem->loadFromFile(std::string(ASSET_PATH) + "/tileSets/DungeonWalls.json");
    textureSystem->loadFromFile(std::string(ASSET_PATH) + "/tileSets/Jungle.json");
    textureSystem->loadFromFile(std::string(ASSET_PATH) + "/tileSets/Graveyard.json");

    int _ = ImGui::SFML::Init(window);
    window.setFramerateLimit(60);

    for (int i = 1000; i < 1500; i++)
    {
        entities[i] = gCoordinator.createEntity();
        gCoordinator.addComponent(entities[i], RenderComponent{});
        gCoordinator.addComponent(entities[i], TileComponent{});
        gCoordinator.addComponent(entities[i], TransformComponent{});
        gCoordinator.addComponent(entities[i], AnimationComponent{});
    }


    std::string s(std::string(ASSET_PATH) + "/maps/map_01.json");
    mapSystem->loadMap(s);

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        sf::Event event{};
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
        {
            std::string s(std::string(ASSET_PATH) + "/maps/map_01.json");
            mapSystem->loadMap(s);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
        {
            std::string s(std::string(ASSET_PATH) + "/maps/map_02.json");
            mapSystem->loadMap(s);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
        {
            std::string s(std::string(ASSET_PATH) + "/maps/map_03.json");
            mapSystem->loadMap(s);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
        {
            std::string s(std::string(ASSET_PATH) + "/maps/map_04.json");
            mapSystem->loadMap(s);
        }

        glm::vec2 dir{};

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) // Move Up
            dir.y -= 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) // Move Down
            dir.y += 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) // Move Right
            dir.x += 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) // Move Left
            dir.x -= 1;
        if (dir.x != 0 || dir.y != 0)
        {
            playerMovementSystem->onMove(dir);
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // Clear the window before drawing
        window.clear();

        // Draw the map tiles
        textureSystem->loadTextures();

        // Draw other entities or systems here if needed
        renderSystem->draw(window);

        // Render ImGui
        ImGui::SFML::Render(window);

        // Display the rendered frame
        window.display();
    }
}
