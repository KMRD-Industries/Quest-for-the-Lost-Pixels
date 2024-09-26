#include "FightSystem.h"

#include "AnimationComponent.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CreateBodyWithCollisionEvent.h"
#include "EquippedWeaponComponent.h"
#include "FightActionEvent.h"
#include "ObjectCreatorSystem.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "TextTagComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

FightSystem::FightSystem()
{
    init();
}

void FightSystem::init()
{

}

void FightSystem::update()
{
    for (const auto& entity : m_entities)
    {
        const auto& [eventEntity] = gCoordinator.getComponent<FightActionEvent>(entity);

        // Start attack animation
        const auto& equippedWeapon = gCoordinator.getComponent<EquippedWeaponComponent>(eventEntity);
        auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(equippedWeapon.currentWeapon);

        switch(weaponComponent.type)
        {
        case GameType::WeaponType::MELE:
            handleMeleAttack(eventEntity);
            break;
        case GameType::WeaponType::WAND:
            handleWandAttack(eventEntity);
            break;
        case GameType::WeaponType::BOW:
            handleBowAttack(eventEntity);
            break;
        default:
            break;
        }
    }

    clear();
}

void FightSystem::handleBowAttack(Entity)
{

}

void FightSystem::handleMeleAttack(Entity eventEntity)
{
    auto& renderComponent = gCoordinator.getComponent<RenderComponent>(eventEntity);
    const auto& equippedWeapon = gCoordinator.getComponent<EquippedWeaponComponent>(eventEntity);
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(equippedWeapon.currentWeapon);

    weaponComponent.isFacingRight = renderComponent.sprite.getScale().x > 0;
    weaponComponent.queuedAttack = true;

    const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(eventEntity);
    const auto center = glm::vec2{transformComponent.position.x, transformComponent.position.y};

    // Calculate the direction vector using the angle in degrees
    const auto direction = glm::vec2{
        glm::cos(glm::radians(-weaponComponent.targetAngleDegrees)), // Cosine of the angle
        glm::sin(glm::radians(-weaponComponent.targetAngleDegrees)) // Sine of the angle
    };

    const auto range = direction * (config::playerAttackRange * std::abs(transformComponent.scale.x));
    const auto point2 = center + range;

    const auto targetInBox = Physics::rayCast(center, point2, eventEntity);

    if (targetInBox.tag == "Enemy")
    {
        auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(targetInBox.entityID);
        const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(targetInBox.entityID);
        auto& secondPlayertransformComponent = gCoordinator.getComponent<TransformComponent>(targetInBox.entityID);

        const Entity tag = gCoordinator.createEntity();
        gCoordinator.addComponent(tag, TextTagComponent{});
        gCoordinator.addComponent(tag, TransformComponent{secondPlayertransformComponent});

        characterComponent.attacked = true;
        characterComponent.hp -= config::playerAttackDamage;

        const b2Vec2& attackerPos = gCoordinator.getComponent<ColliderComponent>(eventEntity).body->GetPosition();
        const b2Vec2& targetPos = colliderComponent.body->GetPosition();

        b2Vec2 recoilDirection = targetPos - attackerPos;
        recoilDirection.Normalize();

        const float& recoilMagnitude = 20.0f;
        const b2Vec2 recoilVelocity = recoilMagnitude * recoilDirection;

        secondPlayertransformComponent.velocity.x +=
            static_cast<float>(recoilVelocity.x * config::meterToPixelRatio);
        secondPlayertransformComponent.velocity.y +=
            static_cast<float>(recoilVelocity.y * config::meterToPixelRatio);

        b2Vec2 newPosition = colliderComponent.body->GetPosition() + 0.25 * recoilDirection;
        colliderComponent.body->SetTransform(newPosition, colliderComponent.body->GetAngle());
    }
}

void FightSystem::handleCollision(const Entity bullet, const GameType::CollisionData& collisionData) const
{
    if (std::regex_match(collisionData.tag, config::playerRegexTag)) return;
    gCoordinator.getComponent<CharacterComponent>(bullet).hp = -1;
}

void FightSystem::handleWandAttack(Entity eventEntity)
{
    const auto& equippedWeapon = gCoordinator.getComponent<EquippedWeaponComponent>(eventEntity);
    const Entity newBulletEntity = gCoordinator.createEntity();

    TransformComponent transformComponent{gCoordinator.getComponent<TransformComponent>(equippedWeapon.currentWeapon)};
    transformComponent.velocity = {1, 0};

    gCoordinator.addComponents(newBulletEntity,
        ColliderComponent{},
        transformComponent,
        RenderComponent{},
        TileComponent{336, "Bullets", 7},
        AnimationComponent{},
        CharacterComponent{}
        );

    const Entity newBulletEvent = gCoordinator.createEntity();

    const auto newEvent = CreateBodyWithCollisionEvent(newBulletEntity
     , "Bullet",
     [this, newBulletEntity](const GameType::CollisionData& collisionData)
     { handleCollision(newBulletEntity, collisionData);}, [this, newBulletEntity](const GameType::CollisionData& collisionData)
     { handleCollision(newBulletEntity, collisionData);},
      false,
      false,
      GameType::ObjectType::BULLET
      );

    gCoordinator.addComponent(newBulletEvent, newEvent);
}


void FightSystem::clear()
{

    {
        std::deque<Entity> entityToRemove;

        for (const auto entity : m_entities)
        {
            entityToRemove.push_back(entity);
        }

        while (!entityToRemove.empty())
        {
            gCoordinator.destroyEntity(entityToRemove.front());
            entityToRemove.pop_front();
        }
    }
}
