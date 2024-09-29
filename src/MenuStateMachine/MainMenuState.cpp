#include "MainMenuState.h"

#include <iostream>

#include "ButtonComponent.h"
#include "ButtonSystem.h"
#include "Coordinator.h"
#include "Dungeon.h"
#include "Paths.h"
#include "TransformComponent.h"
#include "UiComponent.h"

extern Coordinator gCoordinator;

void MainMenuState::init()
{
    gCoordinator.getRegisterSystem<ButtonSystem>()->loadData(std::string(ASSET_PATH) + "/ui/uiHigh.png",
                                                             ASSET_PATH + std::string("/ui/uiFont.ttf"), 40.f);

    const Entity backgroundEntity = gCoordinator.createEntity();
    sf::Texture* backgroundTexture = new sf::Texture();
    if (!backgroundTexture->loadFromFile(std::string(ASSET_PATH) + "/ui/Background.png"))
        return;
    sf::Sprite* backgroundSprite = new sf::Sprite();
    backgroundSprite->setTexture(*backgroundTexture);
    gCoordinator.addComponent(backgroundEntity, UiComponent{backgroundSprite, 1});
    gCoordinator.addComponent(backgroundEntity, TransformComponent{});

    const Entity startButtonEntity = gCoordinator.createEntity();
    gCoordinator.addComponent(startButtonEntity, ButtonComponent{std::string(ASSET_PATH) + "/ui/uiHigh.png",
                                                                 ASSET_PATH + std::string("/ui/uiFont.ttf"), "Start",
                                                                 40.f, 2.f,
                                                                 {818, 850}, {0, 387, 144, 42},
                                                                 {144, 387, 144, 42}, {288, 387, 144, 42},
                                                                 {0, 0}, {1, 1}, [this]()
                                                                 {
                                                                     m_stateChangeCallback(
                                                                         MenuStateMachine::StateAction::Push,
                                                                         std::make_unique<Dungeon>());
                                                                 }
                              });
    const Entity quitButtonEntity = gCoordinator.createEntity();
    gCoordinator.addComponent(quitButtonEntity, ButtonComponent{std::string(ASSET_PATH) + "/ui/uiHigh.png",
                                                                ASSET_PATH + std::string("/ui/uiFont.ttf"), "Quit",
                                                                40.f, 2.f,
                                                                {818, 950}, {0, 387, 144, 42},
                                                                {144, 387, 144, 42}, {288, 387, 144, 42},
                                                                {0, 0}, {1, 1}, []()
                                                                {
                                                                    std::exit(1);
                                                                }
                              });
}

void MainMenuState::render()
{
}

void MainMenuState::update(const float deltaTime)
{
}