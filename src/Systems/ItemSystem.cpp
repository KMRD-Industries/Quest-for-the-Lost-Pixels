#include "ItemSystem.h"
#include <RenderSystem.h>
#include <cmath>
#include "AnimationSystem.h"
#include "ColliderComponent.h"
#include "EquippedWeaponComponent.h"
#include "HelmetComponent.h"
#include "InventorySystem.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

void ItemSystem::init() {}

void ItemSystem::markClosest() {}

void ItemSystem::displayWeaponStats(const Entity entity)
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

void ItemSystem::displayHelmetStats(const Entity entity)
{
    const auto& helmet = gCoordinator.getComponent<HelmetComponent>(entity);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f)); // Semi-transparent background
    ImGui::SetNextWindowPos(ImVec2(10, 100));
    ImGui::SetNextWindowSize(ImVec2(250, 0));
    ImGui::Begin("Pick Up Item Stats", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysUseWindowPadding);

    ImGui::Separator();
    ImGui::Text("Helmet Details");
    ImGui::Separator();

    ImGui::Text("ID: %d", helmet.id);
    ImGui::End();
    ImGui::PopStyleColor();
}

void ItemSystem::update()
{
    const auto& playerTransformComponent = gCoordinator.getComponent<TransformComponent>(config::playerEntity);
    double minDistance = std::numeric_limits<float>::max();
    Entity closestItemEntity{};

    for (const auto entity : m_entities)
    {
        if (const auto* weaponComponent = gCoordinator.tryGetComponent<WeaponComponent>(entity))
            if (weaponComponent->equipped == true) continue;

        if (const auto* itemComponent = gCoordinator.tryGetComponent<HelmetComponent>(entity))
            if (itemComponent->equipped == true) continue;

        const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        const float distanceX = transformComponent.position.x - playerTransformComponent.position.x;
        const float distanceY = transformComponent.position.y - playerTransformComponent.position.y;
        const double distance = std::sqrt(std::pow(distanceX, 2) + std::pow(distanceY, 2));

        if (distance <= minDistance)
        {
            minDistance = distance;
            closestItemEntity = entity;
        }
    }

    if (minDistance <= config::weaponInteractionDistance)
    {
        gCoordinator.getComponent<RenderComponent>(closestItemEntity).color = sf::Color(255, 102, 102);

        if (gCoordinator.hasComponent<WeaponComponent>(closestItemEntity))
            displayWeaponStats(closestItemEntity);
        else if (gCoordinator.hasComponent<HelmetComponent>(closestItemEntity))
            displayHelmetStats(closestItemEntity);
    }

    // markClosest();
}

void ItemSystem::input(const Entity player)
{
    const auto& playerTransformComponent = gCoordinator.getComponent<TransformComponent>(player);
    double minDistance = std::numeric_limits<float>::max();
    Entity closestWeaponEntity{};

    for (const auto entity : m_entities)
    {
        if (const auto* weaponComponent = gCoordinator.tryGetComponent<WeaponComponent>(entity))
            if (weaponComponent->equipped == true) continue;

        if (const auto* itemComponent = gCoordinator.tryGetComponent<HelmetComponent>(entity))
            if (itemComponent->equipped == true) continue;

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
        if (gCoordinator.hasComponent<WeaponComponent>(closestWeaponEntity))
        {
            // gCoordinator.getRegisterSystem<InventorySystem>()->dropWeapon(player);
            gCoordinator.getRegisterSystem<InventorySystem>()->pickUpWeapon(player, closestWeaponEntity);
        }

        else if (gCoordinator.hasComponent<HelmetComponent>(closestWeaponEntity))
        {
            // gCoordinator.getRegisterSystem<InventorySystem>()->dropItem(player);
            gCoordinator.getRegisterSystem<InventorySystem>()->pickUpItem(player, closestWeaponEntity);
        }
    }
}

void ItemSystem::deleteItems() const
{
    for (const auto entity : m_entities)
    {
        if (const auto* weaponComponent = gCoordinator.tryGetComponent<WeaponComponent>(entity))
            if (weaponComponent->equipped == true) continue;

        if (const auto* itemComponent = gCoordinator.tryGetComponent<HelmetComponent>(entity))
            if (itemComponent->equipped == true) continue;

        if (auto* collisionComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity))
            collisionComponent->toDestroy = true;
    }
}