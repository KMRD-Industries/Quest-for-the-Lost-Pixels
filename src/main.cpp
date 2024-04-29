#include <imgui-SFML.h>
#include <imgui.h>

#include <SFML/Graphics.hpp>
#include <iostream>

#include "Coordinator.h"
#include "MapComponent.h"
#include "MapSystem.h"
#include "RenderComponent.h"
#include "RenderSystem.h"
#include "RoomComponent.h"
#include "TileComponent.h"
#include "TransformComponent.h"

Coordinator gCoordinator;

int main() {
    gCoordinator.init();
    gCoordinator.registerComponent<RenderComponent>();
    gCoordinator.registerComponent<TileComponent>();
    gCoordinator.registerComponent<TransformComponent>();
    gCoordinator.registerComponent<MapComponent>();
    gCoordinator.registerComponent<TransformMapComponent>();

    auto renderSystem = gCoordinator.getRegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<RenderComponent>());
        gCoordinator.setSystemSignature<RenderSystem>(signature);
    }

    auto mapSystem = gCoordinator.getRegisterSystem<MapSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<TransformMapComponent>());
        signature.set(gCoordinator.getComponentType<TileComponent>());
        gCoordinator.setSystemSignature<MapSystem>(signature);
    }

    std::vector<Entity> entities(MAX_ENTITIES - 1);
    sf::RenderWindow window(sf::VideoMode(1280, 720), "ImGui + SFML = <3");
    ImGui::SFML::Init(window);
    window.setFramerateLimit(60);

    for(int i = 1000; i < 2000; i++){
        entities[i] = gCoordinator.createEntity();
        gCoordinator.addComponent(entities[i], TileComponent{});
        gCoordinator.addComponent(entities[i], TransformMapComponent{});
    }

    std::string s("../resources/Maps/map_01.json");
    mapSystem->loadMap(s);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                // Check which key is pressed and set the corresponding index in buttonPressed to true
                if (event.key.code == sf::Keyboard::Key::Num1) {
                    s = ("../resources/Maps/map_01.json");
                    mapSystem->loadMap(s);
                } else if (event.key.code == sf::Keyboard::Key::Num2) {
                    s = ("../resources/Maps/map_02.json");
                    mapSystem->loadMap(s);
                } else if (event.key.code == sf::Keyboard::Key::Num3) {
                    s = ("../resources/Maps/map_03.json");
                    mapSystem->loadMap(s);
                } else if (event.key.code == sf::Keyboard::Key::Num4) {
                    s = ("../resources/Maps/map_04.json");
                    mapSystem->loadMap(s);
                }
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // Clear the window before drawing
        window.clear();

        // Draw the map tiles
        mapSystem->draw(window);

        // Draw other entities or systems here if needed
        renderSystem->draw(window);

        // Render ImGui
        ImGui::SFML::Render(window);

        // Display the rendered frame
        window.display();
    }

}
