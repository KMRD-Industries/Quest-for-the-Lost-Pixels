#include "FightSystem.h"

#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CreateBodyWithCollisionEvent.h"
#include "EquipmentComponent.h"
#include "FightActionEvent.h"
#include "ObjectCreatorSystem.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "TextTagComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

extern PublicConfigSingleton configSingleton;

FightSystem::FightSystem() { init(); }

void FightSystem::init()
{
}

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

void FightSystem::handleBowAttack(Entity)
{
}

void FightSystem::handleMeleeAttack(const Entity eventEntity)
{
    auto& renderComponent = gCoordinator.getComponent<RenderComponent>(eventEntity);
    auto& [equipment] = gCoordinator.getComponent<EquipmentComponent>(eventEntity);
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(equipment.at(GameType::slotType::WEAPON));

    weaponComponent.isFacingRight = renderComponent.sprite.getScale().x > 0;
    weaponComponent.queuedAttack = true;

    const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(eventEntity);
    const auto center = glm::vec2{transformComponent.position.x, transformComponent.position.y};

    // Calculate the direction vector using the angle in degrees
    const auto direction = glm::vec2{
        glm::cos(glm::radians(-weaponComponent.targetAngleDegrees)), // Cosine of the angle
        glm::sin(glm::radians(-weaponComponent.targetAngleDegrees)) // Sine of the angle
    };

    const auto range = direction * (configSingleton.GetConfig().playerAttackRange *
        std::abs(transformComponent.scale.x));
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
        characterComponent.hp -= configSingleton.GetConfig().playerAttackDamage;

        const b2Vec2& attackerPos = gCoordinator.getComponent<ColliderComponent>(eventEntity).body->GetPosition();
        const b2Vec2& targetPos = colliderComponent.body->GetPosition();

        b2Vec2 recoilDirection = targetPos - attackerPos;
        recoilDirection.Normalize();

        const float& recoilMagnitude = 20.0f;
        const b2Vec2 recoilVelocity = recoilMagnitude * recoilDirection;

        secondPlayertransformComponent.velocity.x += static_cast<float>(recoilVelocity.x * configSingleton.GetConfig().
            meterToPixelRatio);
        secondPlayertransformComponent.velocity.y += static_cast<float>(recoilVelocity.y * configSingleton.GetConfig().
            meterToPixelRatio);

        b2Vec2 newPosition = colliderComponent.body->GetPosition() + 0.25 * recoilDirection;
        colliderComponent.body->SetTransform(newPosition, colliderComponent.body->GetAngle());
    }
}

void FightSystem::handleCollision(const Entity bullet, const GameType::CollisionData& collisionData) const
{
    if (std::regex_match(collisionData.tag, config::playerRegexTag))
        return;
    if (collisionData.tag == "Bullet")
        return;

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

    for (const auto entity : m_entities)
        entityToRemove.push_back(entity);

    while (!entityToRemove.empty())
    {
        gCoordinator.destroyEntity(entityToRemove.front());
        entityToRemove.pop_front();
    }
}