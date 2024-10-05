#include "WeaponsSystem.h"
#include <RenderSystem.h>
#include <cmath>
#include "AnimationSystem.h"
#include "ColliderComponent.h"
#include "EquippedWeaponComponent.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

void WeaponSystem::update(const float &deltaTime) {
    if (m_frameTime += deltaTime; m_frameTime >= config::oneFrameTimeMs) {
        m_frameTime -= config::oneFrameTimeMs;
        performFixedUpdate();
    }
}

void WeaponSystem::performFixedUpdate() {
    for (const auto entity: m_entities) {
        updateStartingAngle(entity);
        updateWeaponAngle(entity);
    }
}

inline void WeaponSystem::updateWeaponAngle(const Entity entity) {
    auto &weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

    if (!weaponComponent.isAttacking) return;
    rotateWeapon(entity, weaponComponent.isSwingingForward);
}

void WeaponSystem::deleteItems() const {
    std::deque<Entity> entityToRemove;

    for (const auto entity: m_entities) {
        if (gCoordinator.getComponent<WeaponComponent>(entity).equipped == false) {
            if (gCoordinator.hasComponent<ColliderComponent>(entity))
                gCoordinator.getComponent<ColliderComponent>(entity).toDestroy = true;
            else
                entityToRemove.push_back(entity);
        }
    }

    for (const auto entity: entityToRemove) gCoordinator.destroyEntity(entity);
}

inline void WeaponSystem::rotateWeapon(const Entity entity, bool forward) {
    auto &weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

    weaponComponent.remainingDistance -= weaponComponent.rotationSpeed;
    const float direction = weaponComponent.isFacingRight ? 1.f : -1.f;
    const float isMovingForward = forward ? 1.f : -1.f;

    if (weaponComponent.remainingDistance > 0) {
        weaponComponent.currentAngle += weaponComponent.rotationSpeed * direction * isMovingForward;
    } else {
        weaponComponent.currentAngle -= weaponComponent.remainingDistance * direction * isMovingForward;
        weaponComponent.remainingDistance = weaponComponent.swingDistance;
        weaponComponent.isSwingingForward = !forward;

        if (!forward) {
            if (weaponComponent.queuedAttack == true) {
                weaponComponent.isAttacking = true;
                weaponComponent.queuedAttack = false;

                setAngle(entity);
            } else
                weaponComponent.isAttacking = false;
        }
    }
}

inline void WeaponSystem::setAngle(const Entity entity) {
    auto &weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);
    auto &renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

    weaponComponent.remainingDistance = weaponComponent.swingDistance;
    const auto &origin = renderComponent.sprite.getPosition();

    // Calculate the vector from the player position to the mouse position.
    const sf::Vector2f mouseOffset = weaponComponent.pivotPoint - origin;

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

inline void WeaponSystem::updateStartingAngle(const Entity entity) {
    auto &weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

    if (weaponComponent.queuedAttack && !weaponComponent.isAttacking) {
        weaponComponent.isAttacking = true;
        weaponComponent.queuedAttack = false;
        return;
    }

    if (weaponComponent.queuedAttack || weaponComponent.isAttacking) return;

    setAngle(entity);
}
