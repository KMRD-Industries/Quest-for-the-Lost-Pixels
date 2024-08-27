#include "WeaponsSystem.h"
#include <RenderSystem.h>
#include <cmath>
#include "AnimationSystem.h"
#include "RenderComponent.h"
#include "WeaponComponent.h"

void WeaponSystem::update() const
{
    for (const auto entity : m_entities)
    {
        updateStartingAngle(entity);
        updateWeaponAngle(entity);
    }
}

/**
 * Updates the weapon's angle during an attack.
 * If the weapon is attacking, the angle is adjusted by rotating forward or backward.
 * \param entity
 */
void WeaponSystem::updateWeaponAngle(const Entity entity)
{
    const auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

    if (!weaponComponent.isAttacking) return;
    weaponComponent.isSwingingForward ? rotateForward(entity) : rotateBackward(entity);
}

/**
 * \brief Rotates the weapon forward during a top-down sword swing animation.
 * Full distance the sword needs to travel is described by WeaponComponent.swingDistance.
 * Remaining distance, by analogy, is described by remainingDistance.
 * When distance is less than 0 the flag isSwingForward is set to false
 * informing system to rotate back to base position.
 * \param entity
 * */
void WeaponSystem::rotateForward(const Entity entity)
{
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

    // Decrease the remaining distance by the rotation speed.
    // If the remaining distance is <, = 0, animation is stopped.
    weaponComponent.remainingDistance -= weaponComponent.rotationSpeed;

    if (weaponComponent.remainingDistance > 0)
    {
        float adjustedValue = weaponComponent.rotationSpeed;

        // Adjust rotation based on the character's facing direction.
        adjustedValue *= weaponComponent.isFacingRight ? 1 : -1;

        weaponComponent.currentAngle += adjustedValue;
    }
    else
    {
        // Finalize the swing at the bottom position,
        // reset the remaining distance for the next swing, and flag the swing as complete.
        float adjustedValue = -weaponComponent.remainingDistance;

        // Adjust rotation based on the character's facing direction.
        adjustedValue *= weaponComponent.isFacingRight ? 1.f : -1.f;

        weaponComponent.currentAngle += adjustedValue;
        weaponComponent.remainingDistance = weaponComponent.swingDistance;
        weaponComponent.isSwingingForward = false;
    }
}

/**
 * \brief Rotates the weapon backwards during a bottom-up sword swing animation.
 * Full distance the sword needs to travel is described by WeaponComponent.swingDistance.
 * Remaining distance, by analogy, is described by remainingDistance.
 * When distance is less than 0 the flag isSwingForward is set to true.
 * \param entity weaponComponent to modify.
 * */
void WeaponSystem::rotateBackward(const Entity entity)
{
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

    // Decrease the remaining distance by the rotation speed.
    weaponComponent.remainingDistance -= weaponComponent.rotationSpeed;

    if (weaponComponent.remainingDistance > 0)
    {
        // Adjust rotation based on the character's facing direction.
        float adjustedValue = weaponComponent.rotationSpeed;
        adjustedValue *= weaponComponent.isFacingRight ? 1 : -1;

        weaponComponent.currentAngle -= adjustedValue;
    }
    else
    {
        // If the remaining distance is less than 0, finalize the swing at the top position and
        // reset to the original state.
        float adjustedValue = -weaponComponent.remainingDistance;
        adjustedValue *= weaponComponent.isFacingRight ? 1.f : -1.f;

        weaponComponent.currentAngle += adjustedValue;
        weaponComponent.remainingDistance = weaponComponent.swingDistance;
        weaponComponent.isSwingingForward = true;

        // If next attack is queued start next animation immediately
        if (weaponComponent.queuedAttack == true)
        {
            weaponComponent.isAttacking = true;
            weaponComponent.queuedAttack = false;

            setAngle(entity);
        }
        else
        {
            weaponComponent.isAttacking = false;
        }
    }
}

/**
 * \brief Adjust the current angle based on the facing direction and the initial angle.

 * \param entity weaponComponent to modify.
 * */
void WeaponSystem::setAngle(const Entity entity)
{
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);
    const auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

    // Reset the remaining distance when not in animating state.
    weaponComponent.remainingDistance = weaponComponent.swingDistance;

    // Get the position of player.
    const auto& origin = renderComponent.sprite.getPosition();

    // Calculate the vector from the player position to the mouse position.
    sf::Vector2i mousePositionFromPlayer{};
    mousePositionFromPlayer.x = weaponComponent.pivotPoint.x - static_cast<int>(origin.x);
    mousePositionFromPlayer.y = weaponComponent.pivotPoint.y - static_cast<int>(origin.y);

    // Update the target point and determine the facing direction.
    weaponComponent.targetPoint = mousePositionFromPlayer;
    weaponComponent.isFacingRight = weaponComponent.targetPoint.x >= 0;

    const double angle = std::atan2(mousePositionFromPlayer.y, mousePositionFromPlayer.x);

    // Convert radians into an angle.
    auto angleInDegrees = static_cast<float>(angle * (-180.0f / M_PI));
    weaponComponent.targetAngleDegrees = angleInDegrees;

    if (weaponComponent.isFacingRight)
    {
        weaponComponent.currentAngle = 90.f - angleInDegrees - weaponComponent.initialAngle;
    }
    else
    {
        weaponComponent.currentAngle = 90.f - angleInDegrees + weaponComponent.initialAngle;
    }
}

/**
 * \brief Sets the weapon's starting angle when not attacking.
 * The starting angle is determined by the mouse position relative to the player and  initial angle.
 * \param entity
 */
void WeaponSystem::updateStartingAngle(const Entity entity)
{
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

    if (weaponComponent.queuedAttack && !weaponComponent.isAttacking)
    {
        weaponComponent.isAttacking = true;
        weaponComponent.queuedAttack = false;
        return;
    }

    if (weaponComponent.queuedAttack || weaponComponent.isAttacking)
    {
        return;
    }

    if (weaponComponent.isAttacking) return;
    setAngle(entity);
}