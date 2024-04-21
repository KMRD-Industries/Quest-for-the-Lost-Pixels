/*For Generator Preview Start*/
#define GLM_ENABLE_EXPERIMENTAL
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include "DungeonGenerator.h"
/*For Generator Preview End*/
#include "Coordinator.h"

#include <imgui-SFML.h>
#include <imgui.h>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "DungeonGenerator.h"
#include "RenderComponent.h"
#include "RenderSystem.h"

Coordinator gCoordinator;

int main()
{
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
    gCoordinator.addComponent(entities[0], RenderComponent{new sf::CircleShape{1.f}});
    sf::RenderWindow window(sf::VideoMode(1280, 720), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    auto _ = ImGui::SFML::Init(window);

    sf::Clock deltaClock;

    /*Map Generator Preview Start*/
    sf::Font font;
    font.loadFromFile("../Arial.ttf");
    int h = 5;
    int w = 6;
    DungeonGenerator generator(h, w);
    generator.generateMainPath(11);
    generator.generateSidePath(
        {.pathName{"FirstC"}, .startingPathName{"Main"}, .endPathName{"Main"}, .minPathLength{3}, .maxPathLength{5}});
    generator.generateSidePath(
        {.pathName{"SecondC"}, .startingPathName{"Main"}, .endPathName{""}, .minPathLength{3}, .maxPathLength{5}});
    const int rectWidth = 50;
    const int rectHeight = 50;
    const int gap = 20;
    std::vector<sf::RectangleShape> rectangles;
    std::vector<sf::Text> texts;


    auto fullMap = generator.getNodes();
    auto roomCount = generator.getCount();
    generator.makeLockAndKey();
    for (int i = 0; i < w; ++i)
    {
        for (int j = 0; j < h; ++j)
        {
            sf::RectangleShape rectangle(sf::Vector2f(rectWidth, rectHeight));
            rectangle.setPosition(i * (rectWidth + gap), j * (rectHeight + gap));

            if (fullMap[{i, j}] == "Main")
            {
                rectangle.setFillColor(sf::Color::Red);
            }
            else if (fullMap[{i, j}] == "FirstC")
            {
                rectangle.setFillColor(sf::Color::Blue);
            }
            else if (fullMap[{i, j}] == "SecondC")
            {
                rectangle.setFillColor(sf::Color::Green);
            }
            else
            {
                rectangle.setFillColor(sf::Color::Black);
            }
            rectangles.push_back(rectangle);

            auto currentNodeVal = roomCount[{i, j}];
            sf::Text text;
            text.setFont(font);
            text.setString(std::to_string(currentNodeVal));
            text.setCharacterSize(14); // w pikselach
            text.setFillColor(sf::Color::White);
            text.setPosition(i * (rectWidth + gap), j * (rectHeight + gap));

            texts.push_back(text);


            sf::Text key;
            key.setFont(font);
            key.setString(std::to_string(generator.m_nodeOutEdgesCount[{i, j}]));
            key.setCharacterSize(14); // w pikselach
            key.setFillColor(sf::Color::Black);
            key.setPosition(i * (rectWidth + gap) + 30, j * (rectHeight + gap) + 30);
            texts.push_back(key);
            if (const auto text = generator.getKey({i, j}))
            {
                sf::Text key;
                key.setFont(font);
                key.setString(text.value());
                key.setCharacterSize(14); // w pikselach
                key.setFillColor(sf::Color::Black);
                key.setPosition(i * (rectWidth + gap) + 10, j * (rectHeight + gap) + 10);
                texts.push_back(key);
            }
            if (const auto text = generator.getLock({i, j}))
            {
                sf::Text key;
                key.setFont(font);
                key.setString(text.value());
                key.setCharacterSize(14); // w pikselach
                key.setFillColor(sf::Color::Black);
                key.setPosition(i * (rectWidth + gap) + 10, j * (rectHeight + gap) + 10);
                texts.push_back(key);
            }
        }
    }
    /*Map Generator Preview End*/

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

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();

        ImGui::ShowDemoWindow();

        window.clear();
        /*Draw generator start*/
        for (const auto& rectangle : rectangles) window.draw(rectangle);
        for (const auto& text : texts) window.draw(text);
        /*Draw generator end*/

        renderSystem->draw(window);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}
