#include "WeaponsSystem.h"
#include <RenderSystem.h>
#include <cmath>
#include "AnimationSystem.h"
#include "ColliderComponent.h"
#include "DealDMGToEnemyEvent.h"
#include "EquipmentComponent.h"
#include "GameUtility.h"
#include "Physics.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "WeaponSwingComponent.h"

extern PublicConfigSingleton configSingleton;

void WeaponSystem::update(const float& deltaTime)
{
    if (m_frameTime += deltaTime; m_frameTime >= configSingleton.GetConfig().oneFrameTime * 1000)
    {
        m_frameTime -= configSingleton.GetConfig().oneFrameTime * 1000;
        performFixedUpdate();
    }
}

void WeaponSystem::performFixedUpdate()
{
    for (const auto entity : m_entities)
    {
        updateStartingAngle(entity);
        updateWeaponAngle(entity);
    }
}

inline void WeaponSystem::updateWeaponAngle(const Entity entity)
{
    const auto& [equipment] = gCoordinator.getComponent<EquipmentComponent>(entity);
    const auto weaponEntity = equipment.at(GameType::slotType::WEAPON);
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(weaponEntity);

    if (!weaponComponent.isAttacking) return;
    rotateWeapon(entity, weaponComponent.isSwingingForward, weaponEntity);
}


inline void WeaponSystem::rotateWeapon(const Entity entity, bool forward, const Entity weaponEntity)
{
    const auto& [equipment] = gCoordinator.getComponent<EquipmentComponent>(entity);
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(equipment.at(GameType::slotType::WEAPON));

    weaponComponent.remainingDistance -= weaponComponent.rotationSpeed;
    const float direction = weaponComponent.isFacingRight ? 1.f : -1.f;
    const float isMovingForward = forward ? 1.f : -1.f;

    if (weaponComponent.remainingDistance > 0)
        weaponComponent.currentAngle += weaponComponent.rotationSpeed * direction * isMovingForward;
    else
    {
        weaponComponent.currentAngle -= weaponComponent.remainingDistance * direction * isMovingForward;
        weaponComponent.remainingDistance = weaponComponent.swingDistance;
        weaponComponent.isSwingingForward = !forward;

        if (!forward)
        {
            if (weaponComponent.queuedAttack == true)
            {
                dealDMGToCollidedEnemies(weaponEntity, true);
                weaponComponent.isAttacking = true;
                weaponComponent.queuedAttack = false;

                setAngle(entity);
            }
            else
            {
                dealDMGToCollidedEnemies(weaponEntity, true);
                weaponComponent.isAttacking = false;
            }
        }
    }
    auto& weaponTransformComponent = gCoordinator.getComponent<TransformComponent>(entity);
    weaponTransformComponent.rotation = (weaponComponent.currentAngle + 60) * M_PI / 180;
}

inline void WeaponSystem::setAngle(const Entity entity)
{
    const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
    const auto& [equipment] = gCoordinator.getComponent<EquipmentComponent>(entity);
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(equipment.at(GameType::slotType::WEAPON));
    const auto center = sf::Vector2f{transformComponent.position + GameUtility::mapOffset};

    weaponComponent.remainingDistance = weaponComponent.swingDistance;

    // Calculate the vector from the player position to the mouse position.
    const sf::Vector2f mouseOffset = weaponComponent.pivotPoint - center;

    // Update the target point and determine the facing direction.
    weaponComponent.targetPoint = mouseOffset;
    weaponComponent.isFacingRight = mouseOffset.x >= 0;
    const double angleRad = std::atan2(mouseOffset.y, mouseOffset.x);

    constexpr float radToDeg = -180.0f / M_PI;

    // Convert radians into an angle.
    const auto angleInDegrees = static_cast<float>(angleRad * radToDeg);
    weaponComponent.targetAngleDegrees = angleInDegrees;

    // Adjust the current angle based on the facing direction.
    const float adjustedAngle = 90.f - angleInDegrees;
    if (weaponComponent.isFacingRight)
        weaponComponent.currentAngle = adjustedAngle - weaponComponent.initialAngle;
    else
        weaponComponent.currentAngle = adjustedAngle + weaponComponent.initialAngle;
}

inline void WeaponSystem::updateStartingAngle(const Entity entity)
{
    const auto& [equipment] = gCoordinator.getComponent<EquipmentComponent>(entity);
    const auto weaponEntity = equipment.at(GameType::slotType::WEAPON);
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(weaponEntity);

    if (weaponComponent.queuedAttack && !weaponComponent.isAttacking)
    {
        dealDMGToCollidedEnemies(weaponEntity);
        weaponComponent.isAttacking = true;
        weaponComponent.queuedAttack = false;
        return;
    }

    if (weaponComponent.queuedAttack || weaponComponent.isAttacking) return;

    setAngle(entity);
}

void WeaponSystem::dealDMGToCollidedEnemies(const Entity weaponEntity, const bool clear)
{
    auto& swingComponent = gCoordinator.getComponent<WeaponSwingComponent>(weaponEntity);
    for (const Entity enemy : swingComponent.enemyColided)
    {
        if (swingComponent.enemyHited.contains(enemy)) continue;
        gCoordinator.addComponent(enemy, DealDMGToEnemyEvent{});
        swingComponent.enemyHited.insert(enemy);
    }
    if (clear)
    {
        swingComponent.enemyHited.clear();
    }
}