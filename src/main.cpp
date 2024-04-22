#include "Coordinator.h"

#include <imgui-SFML.h>
#include <imgui.h>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include "glm/glm.hpp"

#include "Paths.h"
#include "PlayerComponent.h"
#include "PlayerMovementSystem.h"
#include "RenderComponent.h"
#include "RenderSystem.h"
#include "TransformComponent.h"

Coordinator gCoordinator;

int main()
{
    gCoordinator.init();

    gCoordinator.registerComponent<RenderComponent>();
    gCoordinator.registerComponent<TransformComponent>();
    gCoordinator.registerComponent<PlayerComponent>();

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

    std::vector<Entity> entities(MAX_ENTITIES - 1);
    // Local Player
    entities[0] = gCoordinator.createEntity();
    // Server Player
    entities[1] = gCoordinator.createEntity();
    sf::Texture texture;
    std::string PathToAssets{ASSET_PATH};
    texture.loadFromFile(PathToAssets + "/knight/knight.png");
    gCoordinator.addComponent(entities[0], RenderComponent{.sprite = sf::Sprite(texture)});
    gCoordinator.addComponent(entities[0], TransformComponent{.position = sf::Vector2f(0.f, 0.f)});
    gCoordinator.addComponent(entities[0], PlayerComponent{});

    gCoordinator.addComponent(entities[1], RenderComponent{.sprite = sf::Sprite(texture)});
    gCoordinator.addComponent(entities[1], TransformComponent{.position = sf::Vector2f(10.f, 10.f)});
    sf::RenderWindow window(sf::VideoMode(1280, 720), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    auto _ = ImGui::SFML::Init(window);

    sf::CircleShape shape(100.f);

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

        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();

        ImGui::ShowDemoWindow();

        window.clear();
        renderSystem->draw(window);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}
