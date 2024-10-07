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

void ItemSystem::init() {
}

void ItemSystem::markClosest() {
}

void ItemSystem::displayWeaponStats(const Entity entity) {
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

void ItemSystem::displayHelmetStats(const Entity entity) {
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


void ItemSystem::displayBodyArmourStats(const Entity entity) {
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


void ItemSystem::update() {
    const auto &playerTransformComponent = gCoordinator.getComponent<TransformComponent>(config::playerEntity);
    double minDistance = std::numeric_limits<int>::max();
    Entity closestItemEntity{};
    config::slotType entityType{};

    for (const auto entity: m_entities) {
        if (const auto &item = gCoordinator.getComponent<ItemComponent>(entity); item.equipped == true) continue;

        const auto &transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        const float distanceX = transformComponent.position.x - playerTransformComponent.position.x;
        const float distanceY = transformComponent.position.y - playerTransformComponent.position.y;

        if (const double distance = std::sqrt(std::pow(distanceX, 2) + std::pow(distanceY, 2));
            distance <= minDistance) {
            minDistance = distance;
            closestItemEntity = entity;

            if (gCoordinator.hasComponent<WeaponComponent>(entity)) {
                entityType = config::slotType::WEAPON;
            } else if (gCoordinator.hasComponent<HelmetComponent>(entity)) {
                entityType = config::slotType::HELMET;
            } else if (gCoordinator.hasComponent<BodyArmourComponent>(entity)) {
                entityType = config::slotType::BODY_ARMOUR;
            } else {
                entityType = {};
            }
        }
    }

    if (closestItemEntity == 0) return;

    if (minDistance <= config::weaponInteractionDistance) {
        gCoordinator.getComponent<RenderComponent>(closestItemEntity).color = sf::Color(255, 102, 102);

        switch (entityType) {
            case config::slotType::HELMET:
                displayHelmetStats(closestItemEntity);
                break;
            case config::slotType::WEAPON:
                displayWeaponStats(closestItemEntity);
                break;
            case config::slotType::BODY_ARMOUR:
                displayBodyArmourStats(closestItemEntity);
                break;
            default:
                return;
        }
    }
}

void ItemSystem::input(const Entity player) {
    const auto &playerTransformComponent = gCoordinator.getComponent<TransformComponent>(player);

    double minDistance = std::numeric_limits<float>::max();
    Entity closestWeaponEntity{};

    config::slotType entityType = {};

    for (const auto entity: m_entities) {
        if (gCoordinator.getComponent<ItemComponent>(entity).equipped == true) continue;
        const auto &transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        const float distanceX = transformComponent.position.x - playerTransformComponent.position.x;
        const float distanceY = transformComponent.position.y - playerTransformComponent.position.y;

        if (const double distance = std::sqrt(std::pow(distanceX, 2) + std::pow(distanceY, 2));
            distance <= minDistance) {
            minDistance = distance;
            closestWeaponEntity = entity;

            if (gCoordinator.hasComponent<WeaponComponent>(entity)) {
                entityType = config::slotType::WEAPON;
            } else if (gCoordinator.hasComponent<BodyArmourComponent>(entity)) {
                entityType = config::slotType::BODY_ARMOUR;
            } else if (gCoordinator.hasComponent<HelmetComponent>(entity)) {
                entityType = config::slotType::HELMET;
            }
        }
    }

    if (minDistance < config::weaponInteractionDistance) {
        gCoordinator.getRegisterSystem<InventorySystem>()->pickUpItem(player, closestWeaponEntity, entityType);
    }
}

void ItemSystem::deleteItems() const {
    for (const auto entity: m_entities) {
        if (gCoordinator.getComponent<ItemComponent>(entity).equipped == true) continue;

        if (auto *collisionComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity)) {
            collisionComponent->toDestroy = true;
        }
    }
}
