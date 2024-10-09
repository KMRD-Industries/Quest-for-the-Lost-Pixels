#include "ItemSystem.h"
#include <RenderSystem.h>
#include <cmath>

#include "AnimationSystem.h"
#include "BodyArmourComponent.h"
#include "ColliderComponent.h"
#include "HelmetComponent.h"
#include "InventorySystem.h"
#include "ItemComponent.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

void ItemSystem::init()
{
    m_closestEntity = {};
    m_closestEntitySlotType = {};
}

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
    m_closestEntity = {};
    m_closestEntitySlotType = {};

    for (const auto entity : m_entities)
    {
        if (const auto &item = gCoordinator.getComponent<ItemComponent>(entity); item.equipped == true) continue;

        const auto &transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        const float distanceX = transformComponent.position.x - playerTransformComponent.position.x;
        const float distanceY = transformComponent.position.y - playerTransformComponent.position.y;

        if (const double distance = std::sqrt(std::pow(distanceX, 2) + std::pow(distanceY, 2)); distance <= minDistance)
        {
            minDistance = distance;
            m_closestEntity = entity;

            if (gCoordinator.hasComponent<WeaponComponent>(entity))
            {
                m_closestEntitySlotType = config::slotType::WEAPON;
            }
            else if (gCoordinator.hasComponent<HelmetComponent>(entity))
            {
                m_closestEntitySlotType = config::slotType::HELMET;
            }
            else if (gCoordinator.hasComponent<BodyArmourComponent>(entity))
            {
                m_closestEntitySlotType = config::slotType::BODY_ARMOUR;
            }
            else
            {
                m_closestEntitySlotType = {};
            }
        }
    }

    if (m_closestEntity == Entity{}) return;

    if (minDistance <= config::weaponInteractionDistance)
    {
        gCoordinator.getComponent<RenderComponent>(m_closestEntity).color = sf::Color(255, 102, 102);

        switch (m_closestEntitySlotType)
        {
        case config::slotType::HELMET:
            displayHelmetStats(m_closestEntity);
            break;
        case config::slotType::WEAPON:
            displayWeaponStats(m_closestEntity);
            break;
        case config::slotType::BODY_ARMOUR:
            displayBodyArmourStats(m_closestEntity);
            break;
        default:
            break;
        }
    }
    else
    {
        m_closestEntitySlotType = {};
        m_closestEntity = {};
    }
}

void ItemSystem::input(const Entity player)
{
    if (m_closestEntity == Entity{} || m_closestEntitySlotType == config::slotType{}) return;
    gCoordinator.getRegisterSystem<InventorySystem>()->pickUpItem(player, m_closestEntity, m_closestEntitySlotType);
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
