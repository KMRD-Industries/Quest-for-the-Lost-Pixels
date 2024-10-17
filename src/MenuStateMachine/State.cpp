#pragma once
#include "State.h"

#include <CreateBodyWithCollisionEvent.h>
#include <FightActionEvent.h>
#include <RenderSystem.h>

#include "BackgroundSystem.h"
#include "ButtonComponent.h"
#include "ButtonSystem.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "Coordinator.h"
#include "FightActionEvent.h"
#include "FightSystem.h"
#include "ObjectCreatorSystem.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "UiComponent.h"

extern Coordinator gCoordinator;

void State::beforeInit()
{
    if (!m_resetECS)
        return;

    Physics::getInstance()->resetWorld();
    gCoordinator.init();

    gCoordinator.registerComponent<ButtonComponent>();
    gCoordinator.registerComponent<ColliderComponent>();
    gCoordinator.registerComponent<RenderComponent>();
    gCoordinator.registerComponent<TransformComponent>();
    gCoordinator.registerComponent<CreateBodyWithCollisionEvent>();
    gCoordinator.registerComponent<UiComponent>();
    gCoordinator.registerComponent<FightActionEvent>();

    auto collisionSystem = gCoordinator.getRegisterSystem<CollisionSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<RenderComponent>());
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        signature.set(gCoordinator.getComponentType<ColliderComponent>());
        gCoordinator.setSystemSignature<CollisionSystem>(signature);
    }

    auto renderSystem = gCoordinator.getRegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<RenderComponent>());
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        gCoordinator.setSystemSignature<RenderSystem>(signature);
    }

    auto backgroundSystem = gCoordinator.getRegisterSystem<BackgroundSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<UiComponent>());
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        gCoordinator.setSystemSignature<BackgroundSystem>(signature);
    }

    auto buttonSystem = gCoordinator.getRegisterSystem<ButtonSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<ButtonComponent>());
        gCoordinator.setSystemSignature<ButtonSystem>(signature);
    }

    auto objectCreatorSystem = gCoordinator.getRegisterSystem<ObjectCreatorSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<CreateBodyWithCollisionEvent>());
        gCoordinator.setSystemSignature<ObjectCreatorSystem>(signature);
    }

    auto fightSystem = gCoordinator.getRegisterSystem<FightSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<FightActionEvent>());
        gCoordinator.setSystemSignature<FightSystem>(signature);
    }
}