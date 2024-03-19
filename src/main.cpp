#include "Coordinator.h"

#include <imgui-SFML.h>
#include <imgui.h>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "RenderSystem.h"
#include "RenderComponent.h"

Coordinator gCoordinator;

int main() {
    gCoordinator.init();

    gCoordinator.registerComponent<RenderComponent>();

    auto renderSystem = gCoordinator.getRegisterSystem<RenderSystem>();
    {
	Signature signature;
	signature.set(gCoordinator.getComponentType<RenderComponent>());
	gCoordinator.setSystemSignature<RenderSystem>(signature);
    }

    std::vector<Entity> entities(MAX_ENTITIES - 1);

    entities[0] = gCoordinator.createEntity();
    gCoordinator.addComponent(entities[0], RenderComponent{new sf::CircleShape{100.f}});
    sf::RenderWindow window(sf::VideoMode(1280, 720), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    auto _ = ImGui::SFML::Init(window);

    sf::CircleShape shape(100.f);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
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
