#include "Coordinator.h"

#include <imgui-SFML.h>
#include <imgui.h>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "RenderSystem.h"
#include "RenderComponent.h"
#include "TextureAtlas.h"
#include "SpriteBatch.h"


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
    sf::Clock clock;

    moony::TextureAtlas texture_atlas;
    moony::SpriteBatch sprite_batch;
    std::vector<moony::Sprite> sprites;

    if(!texture_atlas.loadFromFile("./../resources/FirstFloorAtlas/example.mtpf")){
        std::cout << "Could not load from atlas!";
        return -1;
    }

    std::vector<std::string> tex_names = texture_atlas.getSubTextureNames();

    for(size_t index = 0; index < 24000; index++)
    {
        size_t name_index = random() % tex_names.size();
        moony::Texture sub_texture = texture_atlas.findSubTexture(tex_names[name_index]);
        moony::Sprite sprite(sub_texture, random()%1000);

        sprite.setPosition(static_cast<float>(random()% 800), static_cast<float>(random()% 600));
        sprite.setOrigin(sprite.m_subtexture.m_rect.width / 2, sprite.m_subtexture.m_rect.height / 2);
        sprites.push_back(sprite);
    }


    sf::Clock deltaClock;
    while (window.isOpen()) {
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

        moony::Texture sub_texture = texture_atlas.findSubTexture("01.png");
        moony::Sprite sprite(sub_texture, 1);
        sprites.push_back(sprite);
        sprite.setPosition(std::rand() % 800, std::rand() % 600);

        float cos = std::cos(clock.getElapsedTime().asSeconds());
        float sin = std::sin(clock.getElapsedTime().asSeconds());

        for(moony::Sprite& sprite : sprites){
            sprite.rotate(cos * sprite.m_layer);
            sprite.move(cos, sin * sprite.m_layer * 0.25);
            // sprite.setScale(sin * 2, sin * 2);
            // sprite.m_layer = random() % 10;
        }

        ImGui::ShowDemoWindow();

        window.clear();
        renderSystem->draw(window);
        sprite_batch.clear();

        for(const moony::Sprite& sprite : sprites)
            sprite_batch.draw(sprite);

        window.draw(sprite_batch);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}