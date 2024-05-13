#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>
#include "AnimationComponent.h"
#include "Coordinator.h"
#include "DungeonGenerator.h"
#include "InputHandler.h"
#include "MapComponent.h"
#include "MapSystem.h"
#include "Paths.h"
#include "PlayerComponent.h"
#include "PlayerMovementSystem.h"
#include "RenderComponent.h"
#include "RenderSystem.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

Coordinator gCoordinator;

void previevGenerator(sf::Font& font, std::vector<sf::RectangleShape>& rectangles, std::vector<sf::Text>& texts)
{
    /*Map Generator Preview Start*/
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
}

int main()
{
    gCoordinator.init();
    gCoordinator.registerComponent<MapComponent>();
    gCoordinator.registerComponent<PlayerComponent>();
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

    sf::Font font;
    const std::string assetPath = ASSET_PATH;
    font.loadFromFile(assetPath + "/fonts/Bentinck-Regular.ttf");

    std::vector<sf::RectangleShape> rectangles;
    std::vector<sf::Text> texts;

    previevGenerator(font, rectangles, texts);

    while (window.isOpen())
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
            else if (event.type == sf::Event::KeyReleased)
            {
                const auto keyCode = event.key.code;
                InputHandler::getInstance()->handleKeyboardInput(keyCode, false);
            }
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
        {
            s = std::string(ASSET_PATH) + "/maps/map_01.json";
            mapSystem->loadMap(s);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
        {
            s = std::string(ASSET_PATH) + "/maps/map_02.json";
            mapSystem->loadMap(s);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
        {
            s = std::string(ASSET_PATH) + "/maps/map_03.json";
            mapSystem->loadMap(s);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
        {
            s = std::string(ASSET_PATH) + "/maps/map_07.json";
            mapSystem->loadMap(s);
        }

        playerMovementSystem->update();

        ImGui::SFML::Update(window, deltaClock.restart());
        // Clear the window before drawing
        window.clear();

        textureSystem->loadTextures();

        renderSystem->draw(window);

        for (const auto& rectangle : rectangles) window.draw(rectangle);
        for (const auto& text : texts) window.draw(text);

        // Render ImGui
        ImGui::SFML::Render(window);
        // Display the rendered frame
        window.display();
    }
}