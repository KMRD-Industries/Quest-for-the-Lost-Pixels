#include "ItemSystem.h"
#include <RenderSystem.h>
#include <cmath>
#include <cstddef>

#include "AnimationSystem.h"
#include "BodyArmourComponent.h"
#include "ChestComponent.h"
#include "ColliderComponent.h"
#include "Config.h"
#include "HelmetComponent.h"
#include "InventorySystem.h"
#include "ItemComponent.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "SpawnerSystem.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

void ItemSystem::init() {}

void ItemSystem::markClosest() {}

void ItemSystem::displayWeaponStats(const Entity entity)
{
    const auto &weapon = gCoordinator.getComponent<WeaponComponent>(entity);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f)); // Semi-transparent background
    ImGui::SetNextWindowPos(ImVec2(10, 100));
    ImGui::SetNextWindowSize(ImVec2(250, 0));
    ImGui::Begin("Pick Up Weapon Stats", nullptr, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoResize);

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
    const auto &helmet = gCoordinator.getComponent<HelmetComponent>(entity);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f)); // Semi-transparent background
    ImGui::SetNextWindowPos(ImVec2(10, 100));
    ImGui::SetNextWindowSize(ImVec2(250, 0));
    ImGui::Begin("Pick Up Item Stats", nullptr, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoResize);

    ImGui::Separator();
    ImGui::Text("Helmet Details");
    ImGui::Separator();

    ImGui::Text("ID: %d", helmet.id);
    ImGui::End();
    ImGui::PopStyleColor();
}


void ItemSystem::displayBodyArmourStats(const Entity entity)
{
    const auto &bodyArmour = gCoordinator.getComponent<BodyArmourComponent>(entity);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f)); // Semi-transparent background
    ImGui::SetNextWindowPos(ImVec2(10, 100));
    ImGui::SetNextWindowSize(ImVec2(250, 0));
    ImGui::Begin("Pick Up Item Stats", nullptr, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoResize);

    ImGui::Separator();
    ImGui::Text("Helmet Details");
    ImGui::Separator();

    ImGui::Text("ID: %d", bodyArmour.id);
    ImGui::End();
    ImGui::PopStyleColor();
}


void ItemSystem::update()
{
    const auto &playerTransformComponent = gCoordinator.getComponent<TransformComponent>(config::playerEntity);
    double minDistance = std::numeric_limits<int>::max();
    closestItem = {};

    for (const auto entity : m_entities)
    {
        if (const auto &item = gCoordinator.getComponent<ItemComponent>(entity); item.equipped == true) continue;

        const auto &transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        const float distanceX = transformComponent.position.x - playerTransformComponent.position.x;
        const float distanceY = transformComponent.position.y - playerTransformComponent.position.y;

        if (const double distance = std::sqrt(std::pow(distanceX, 2) + std::pow(distanceY, 2)); distance <= minDistance)
        {
            minDistance = distance;
            closestItem.itemEntity = entity;

            if (gCoordinator.hasComponent<WeaponComponent>(entity))
            {
                closestItem.slot = GameType::slotType::WEAPON;
            }
            else if (gCoordinator.hasComponent<HelmetComponent>(entity))
            {
                closestItem.slot = GameType::slotType::HELMET;
            }
            else if (gCoordinator.hasComponent<BodyArmourComponent>(entity))
            {
                closestItem.slot = GameType::slotType::BODY_ARMOUR;
            }
            else
            {
                closestItem = {};
            }
        }
    }

    if (closestItem.itemEntity == 0 || gCoordinator.hasComponent<ChestComponent>(closestItem.itemEntity))
    {
        closestItem = {};
        return;
    }

    if (minDistance <= config::weaponInteractionDistance)
    {
        gCoordinator.getComponent<RenderComponent>(closestItem.itemEntity).color = sf::Color(255, 102, 102);

        switch (closestItem.slot)
        {
        case GameType::slotType::HELMET:
            displayHelmetStats(closestItem.itemEntity);
            break;
        case GameType::slotType::WEAPON:
            displayWeaponStats(closestItem.itemEntity);
            break;
        case GameType::slotType::BODY_ARMOUR:
            displayBodyArmourStats(closestItem.itemEntity);
            break;
        default:
            return;
        }
    }
    else
    {
        closestItem = {};
    }
}

void ItemSystem::input(const Entity player)
{
    if (closestItem.itemEntity == 0 || closestItem.slot == GameType::slotType{}) return;
    closestItem.characterEntity = player;
    gCoordinator.getRegisterSystem<InventorySystem>()->pickUpItem(closestItem);
}

void ItemSystem::deleteItems() const
{
    for (const auto entity : m_entities)
    {
        if (gCoordinator.getComponent<ItemComponent>(entity).equipped == true) continue;

        if (auto *collisionComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity))
        {
            collisionComponent->toDestroy = true;
        }
    }
}
