#include <imgui-SFML.h>
#include <imgui.h>
#include <mwaitxintrin.h>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

#include "Coordinator.h"
#include "RenderComponent.h"
#include "RenderSystem.h"
#include "TextureAtlas.h"

Coordinator gCoordinator;

int main() {
    gCoordinator.init();
    gCoordinator.registerComponent<RenderComponent>();

    auto renderSystem = gCoordinator.getRegisterSystem<RenderSystem>();{
        Signature signature;
        signature.set(gCoordinator.getComponentType<RenderComponent>());
        gCoordinator.setSystemSignature<RenderSystem>(signature);
    }

    std::vector<Entity> entities(MAX_ENTITIES - 1);

    sf::Clock frame_clock;
    sf::Time frame_limit = sf::seconds(1.0f / 60.0f);
    sf::Time frame_sum;
    sf::Time frame_time;
    int frame_count;

    entities[0] = gCoordinator.createEntity();

    sf::RenderWindow window(sf::VideoMode(1280, 720), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    auto _ = ImGui::SFML::Init(window);

    sf::CircleShape shape(100.f);
    sf::Clock clock;

    moony::TextureAtlas texture_atlas;


    if(!texture_atlas.loadFromFile("./../resources/Maps/Maps.mtpf")){
        std::cout << "Could not load from atlas!";
        return -1;
    }

    std::vector<std::string> tex_names = texture_atlas.getSubTextureNames();

    entities[1] = gCoordinator.createEntity();
    moony::Texture test = texture_atlas.findSubTexture("room01.png");
    auto new_sprite = sf::Sprite(*test.m_texture, test.m_rect);
    new_sprite.setScale(3., 3.);
    gCoordinator.addComponent(entities[1], RenderComponent{ &new_sprite } );


    sf::Clock deltaClock;
    while (window.isOpen()) {

        frame_time = frame_clock.restart();

        sf::Event event{};
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window ,event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();
        ImGui::ShowDemoWindow();

        frame_sum += frame_time;
        frame_count = 0;

        while(frame_sum > frame_limit && frame_count < 10) {
            frame_sum -= frame_limit;
            frame_count++;
        }
        ImGui::SFML::Render(window);
        window.clear(sf::Color::Black);
        renderSystem->draw(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}