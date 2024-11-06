#include "FightSystem.h"

#include <EquipmentComponent.h>

#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CreateBodyWithCollisionEvent.h"
#include "FightActionEvent.h"
#include "MultiplayerSystem.h"
#include "ObjectCreatorSystem.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "TextTagComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

extern PublicConfigSingleton configSingleton;

FightSystem::FightSystem() { init(); }

void FightSystem::init() {}

void FightSystem::update()
{
    for (const auto entity : m_entities)
    {
        const auto& [eventEntity] = gCoordinator.getComponent<FightActionEvent>(entity);

        // Start attack animation
        const auto& equippedWeapon = gCoordinator.getComponent<EquipmentComponent>(eventEntity);
        const auto& weaponComponent =
            gCoordinator.getComponent<WeaponComponent>(equippedWeapon.slots.at(GameType::slotType::WEAPON));

        switch (weaponComponent.type)
        {
        case GameType::WeaponType::MELE:
            handleMeleeAttack(eventEntity);
            break;
        case GameType::WeaponType::WAND:
            // TODO: Wand attack support
            handleMeleeAttack(eventEntity);
            break;
        case GameType::WeaponType::BOW:
            // TODO: Bow attack support
            handleMeleeAttack(eventEntity);
            break;
        default:
            handleMeleeAttack(eventEntity);
            break;
        }
    }

    clear();
}

void FightSystem::handleBowAttack(Entity) {}

void FightSystem::handleMeleeAttack(const Entity playerEntity) const
{
    const auto& playerRenderComponent = gCoordinator.getComponent<RenderComponent>(playerEntity);
    const auto& [playerEquippedWeapon] = gCoordinator.getComponent<EquipmentComponent>(playerEntity);
    auto& weaponComponent =
        gCoordinator.getComponent<WeaponComponent>(playerEquippedWeapon.at(GameType::slotType::WEAPON));

    weaponComponent.isFacingRight = playerRenderComponent.sprite.getScale().x > 0;
    weaponComponent.queuedAttack = true;
}

void FightSystem::handleCollision(const Entity bullet, const GameType::CollisionData& collisionData) const
{
    if (std::regex_match(collisionData.tag, config::playerRegexTag)) return;
    if (collisionData.tag == "Bullet") return;

    if (collisionData.tag == "Enemy")
    {
        auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(collisionData.entityID);
        auto& enemyTransformComponent = gCoordinator.getComponent<TransformComponent>(collisionData.entityID);

        const Entity tag = gCoordinator.createEntity();
        gCoordinator.addComponent(tag, TextTagComponent{});
        gCoordinator.addComponent(tag, TransformComponent{enemyTransformComponent});

        characterComponent.attacked = true;
        characterComponent.hp -= configSingleton.GetConfig().playerAttackDamage;
    }

    gCoordinator.getComponent<CharacterComponent>(bullet).hp = -1;
}

float FightSystem::calculateAngle(const sf::Vector2f& pivotPoint, const sf::Vector2f& targetPoint) const
{
    const sf::Vector2f direction = targetPoint - pivotPoint;
    return std::atan2(direction.y, direction.x);
}

void FightSystem::handleWandAttack(const Entity eventEntity) const
{
    // TODO: Wand attack support
}

void FightSystem::clear()
{
    std::deque<Entity> entityToRemove;

    for (const auto entity : m_entities) entityToRemove.push_back(entity);

    while (!entityToRemove.empty())
    {
        gCoordinator.destroyEntity(entityToRemove.front());
        entityToRemove.pop_front();
    }
}