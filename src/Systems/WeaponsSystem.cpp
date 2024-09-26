#include "WeaponsSystem.h"
#include <RenderSystem.h>
#include <cmath>
#include "AnimationSystem.h"
#include "ColliderComponent.h"
#include "EquippedWeaponComponent.h"
#include "InventoryComponent.h"
#include "InventorySystem.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

#define M_PI 3.14159265358979323846

void WeaponSystem::init() {}

void WeaponSystem::update()
{
    const auto& playerTransformComponent = gCoordinator.getComponent<TransformComponent>(config::playerEntity);
    double minDistance = std::numeric_limits<float>::max();
    Entity closestWeaponEntity{};

    for (const auto entity : m_entities)
    {
        updateStartingAngle(entity);
        updateWeaponAngle(entity);

        if(gCoordinator.getComponent<WeaponComponent>(entity).equipped == true) continue;

        const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        const float distanceX = transformComponent.position.x - playerTransformComponent.position.x;
        const float distanceY = transformComponent.position.y - playerTransformComponent.position.y;
        const double distance = std::sqrt(std::pow(distanceX, 2) + std::pow(distanceY, 2) <= minDistance);

        if(distance <= minDistance)
        {
            minDistance = distance;
            closestWeaponEntity = entity;
        }
    }

    if(minDistance <= config::weaponInteractionDistance)
    {
        gCoordinator.getComponent<RenderComponent>(closestWeaponEntity).color = sf::Color(255, 102, 102);
        displayStats(closestWeaponEntity);
    }

    // markClosest();
}

void WeaponSystem::displayStats(const Entity entity)
{
    const auto& weapon = gCoordinator.getComponent<WeaponComponent>(entity);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f)); // Semi-transparent background
    ImGui::SetNextWindowPos(ImVec2(10, 100));
    ImGui::SetNextWindowSize(ImVec2(250, 0));
    ImGui::Begin("Pick Up Weapon Stats", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysUseWindowPadding);

    ImGui::Separator();
    ImGui::Text("Weapon Details");
    ImGui::Separator();

    ImGui::Text("ID: %d", weapon.id);
    ImGui::Text("Damage: %d", weapon.damageAmount);
    ImGui::Text("Rotation Speed: %.2f degrees/sec", weapon.rotationSpeed);
    ImGui::Text("Recoil Amount: %.2f", weapon.recoilAmount);

    ImGui::End();
    ImGui::PopStyleColor();
}

void WeaponSystem::weaponInput(const Entity player)
{
    const auto& playerTransformComponent = gCoordinator.getComponent<TransformComponent>(player);
    double minDistance = std::numeric_limits<float>::max();
    Entity closestWeaponEntity{};

    for (const auto entity : m_entities)
    {
        if (gCoordinator.getComponent<WeaponComponent>(entity).equipped == true) continue;

        const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        float distanceX = transformComponent.position.x - playerTransformComponent.position.x;
        float distanceY = transformComponent.position.y - playerTransformComponent.position.y;

        double distance = std::sqrt(std::pow(distanceX, 2) + std::pow(distanceY, 2));

        if (distance <= minDistance)
        {
            minDistance = distance;
            closestWeaponEntity = entity;
        }
    }

    if (minDistance < config::weaponInteractionDistance)
    {
        gCoordinator.getRegisterSystem<InventorySystem>()->dropWeapon(player);
        gCoordinator.getRegisterSystem<InventorySystem>()->pickUpWeapon(player, closestWeaponEntity);
    }
}

inline void WeaponSystem::updateWeaponAngle(const Entity entity)
{
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

    if (!weaponComponent.isAttacking) return;
    rotateWeapon(entity, weaponComponent.isSwingingForward);
}

void WeaponSystem::deleteItems()
{
    std::deque<Entity> entityToRemove;

    for (const auto entity : m_entities)
    {
        if (gCoordinator.getComponent<WeaponComponent>(entity).equipped == false)
        {
            if (gCoordinator.hasComponent<ColliderComponent>(entity))
                gCoordinator.getComponent<ColliderComponent>(entity).toDestroy = true;
            else
                entityToRemove.push_back(entity);
        }
    }

    for (const auto entity : entityToRemove) gCoordinator.destroyEntity(entity);
}


inline void WeaponSystem::rotateWeapon(const Entity entity, bool forward)
{
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

    weaponComponent.remainingDistance -= weaponComponent.rotationSpeed;
    const float direction = weaponComponent.isFacingRight ? 1.f : -1.f;
    const float isMovingForward = forward ? 1.f : -1.f;

    if (weaponComponent.remainingDistance > 0)
    {
        weaponComponent.currentAngle += weaponComponent.rotationSpeed * direction * isMovingForward;
    }
    else
    {
        weaponComponent.currentAngle -= weaponComponent.remainingDistance * direction * isMovingForward;
        weaponComponent.remainingDistance = weaponComponent.swingDistance;
        weaponComponent.isSwingingForward = !forward;

        if (!forward)
        {
            if (weaponComponent.queuedAttack == true)
            {
                weaponComponent.isAttacking = true;
                weaponComponent.queuedAttack = false;

                setAngle(entity);
            }
            else
                weaponComponent.isAttacking = false;
        }
    }
}

inline void WeaponSystem::setAngle(const Entity entity)
{
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);
    auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

    // Reset the remaining distance when not in animating state.
    weaponComponent.remainingDistance = weaponComponent.swingDistance;

    // Get the position of player.
    const auto& origin = renderComponent.sprite.getPosition();

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

inline void WeaponSystem::updateStartingAngle(const Entity entity)
{
    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

    if (weaponComponent.queuedAttack && !weaponComponent.isAttacking)
    {
        weaponComponent.isAttacking = true;
        weaponComponent.queuedAttack = false;
        return;
    }

    if (weaponComponent.queuedAttack || weaponComponent.isAttacking) return;

    setAngle(entity);
}