#include "MainMenuState.h"

#include "ButtonComponent.h"
#include "ButtonSystem.h"
#include "Coordinator.h"
#include "Dungeon.h"
#include "Paths.h"

extern Coordinator gCoordinator;

void MainMenuState::init()
{
    gCoordinator.getRegisterSystem<ButtonSystem>()->loadData(std::string(ASSET_PATH) + "/ui/uiHigh.png",
                                                             ASSET_PATH + std::string("/ui/uiFont.ttf"), 30.f);

    const Entity buttonEntity = gCoordinator.createEntity();
    gCoordinator.addComponent(buttonEntity, ButtonComponent{std::string(ASSET_PATH) + "/ui/uiHigh.png",
                                                            ASSET_PATH + std::string("/ui/uiFont.ttf"), "Multiplayer",
                                                            30.f, 2.f,
                                                            {100, 100}, {0, 387, 144, 42},
                                                            {144, 387, 144, 42}, {288, 387, 144, 42},
                                                            {0, 0}, {1, 1}, [this]()
                                                            {
                                                                m_stateChangeCallback(
                                                                    MenuStateMachine::StateAction::Push,
                                                                    std::make_unique<Dungeon>());
                                                            }
                              });
}

void MainMenuState::render()
{
}

void MainMenuState::update(const float deltaTime)
{
}