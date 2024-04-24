#include <imgui-SFML.h>
#include <imgui.h>

#include <SFML/Graphics.hpp>
#include <iostream>

#include "Coordinator.h"
#include "MapComponent.h"
#include "MapSystem.h"
#include "RenderComponent.h"
#include "RenderSystem.h"
#include "Room.h"
#include "TextureAtlas.h"
#include "TransformComponent.h"

Coordinator gCoordinator;

int main() {
    gCoordinator.init();
    gCoordinator.registerComponent<RenderComponent>();
    gCoordinator.registerComponent<MapComponent>();
    gCoordinator.registerComponent<TransformComponent>();
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
        signature.set(gCoordinator.getComponentType<MapComponent>());
        gCoordinator.setSystemSignature<MapSystem>(signature);
    }

    std::vector<Entity> entities(MAX_ENTITIES - 1);
    sf::RenderWindow window(sf::VideoMode(1280, 720), "ImGui + SFML = <3");
    ImGui::SFML::Init(window);
    window.setFramerateLimit(60);

    Room r;
    std::string s("/home/dominiq/Desktop/KMDR/Quest-for-the-Lost-Pixels/resources/Maps/map_01.json");
    r.load(s);

    int i = 0;

    for (Layer layer : r.getLayers()){
        for (const auto& tilePtr : layer.getTiles()) {
            Tile tile = *tilePtr;
            if(tile.getId() == -1)
                continue;

            entities[i] = gCoordinator.createEntity();
            gCoordinator.addComponent(entities[i], MapComponent{.id = tile.getId()} );
            gCoordinator.addComponent(entities[i], TransformMapComponent{tile.getPosition(), tile.getRotation(), tile.getScale()});

            i++;
        }
    }

    auto layers = r.getLayers();
    std::cout << layers.size();

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // Clear the window before drawing
        window.clear();

        // Draw the map tiles


        // Draw other entities or systems here if needed
        renderSystem->draw(window);
        mapSystem->draw(window);
        // mapSystem->drawTile(window, 1, sf::Vector2f(1, 2));

        // Render ImGui
        ImGui::SFML::Render(window);

        // Display the rendered frame
        window.display();
    }

}
