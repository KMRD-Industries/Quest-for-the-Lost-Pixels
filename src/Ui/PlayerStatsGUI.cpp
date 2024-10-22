#include "PlayerStatsGUI.h"
#include <EnemySystem.h>
#include "CollisionSystem.h"
#include "Coordinator.h"
#include "EquipmentComponent.h"
#include "GameUtility.h"
#include "RenderComponent.h"
#include "RenderSystem.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "TileComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "imgui.h"

extern Coordinator gCordinator;

void PlayerStatsGUI::displayPlayerStatsTable(const sf::RenderWindow& window, const Entity entity) const
{
    // Remove ImGuiCond_Always to allow user to reposition the window manually
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window.getSize().x) - 300, 370), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(270, 350), ImGuiCond_FirstUseEver); // Set width to 270, height is auto
    ImGui::Begin("Player Stats"); // Removed NoResize and NoTitleBar flags

    const auto pos = gCoordinator.getComponent<TransformComponent>(entity);
    const auto& tile = gCoordinator.getComponent<TileComponent>(entity);
    const auto& render = gCoordinator.getComponent<RenderComponent>(entity);

    ImGui::Separator();
    ImGui::Text("Player Movement");
    ImGui::Separator();

    ImGui::Text("Position: (X: %.2f, Y: %.2f)", pos.position.x, pos.position.y);
    ImGui::Text("Velocity: (X: %.2f, Y: %.2f)", pos.velocity.x, pos.velocity.y);

    ImGui::Separator();
    ImGui::Text("Sprite Information");
    ImGui::Separator();

    ImGui::Text("Sprite Position: (X: %.0f, Y: %.0f)", render.sprite.getGlobalBounds().left,
                render.sprite.getGlobalBounds().top);
    ImGui::Text("Sprite Size: Width: %.0f, Height: %.0f", render.sprite.getGlobalBounds().width,
                render.sprite.getGlobalBounds().height);
    ImGui::Text("Tile ID: %.0f", tile.id);

    ImGui::End();
}


void PlayerStatsGUI::displayWeaponStatsTable(const sf::RenderWindow& window, const Entity entity)
{
    const auto& equipment = gCoordinator.getComponent<EquipmentComponent>(entity);
    if (!equipment.slots.contains(GameType::slotType::WEAPON)) return;

    const auto& weapon = gCoordinator.getComponent<WeaponComponent>(equipment.slots.at(GameType::slotType::WEAPON));

    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window.getSize().x) - 300, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(270, 500), ImGuiCond_FirstUseEver);
    ImGui::Begin("Weapon Stats", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysUseWindowPadding);

    ImGui::Separator();
    ImGui::Text("Weapon Details");
    ImGui::Separator();

    ImGui::Text("ID: %d", weapon.id);
    ImGui::Text("Damage: %d", weapon.damageAmount);

    ImGui::Separator();
    ImGui::Text("Status Flags");
    ImGui::Separator();

    ImGui::Text("Is Attacking: %s", weapon.isAttacking ? "Yes" : "No");
    ImGui::Text("Attack Queued: %s", weapon.queuedAttack ? "Yes" : "No");
    ImGui::Text("Swinging Forward: %s", weapon.isSwingingForward ? "Yes" : "No");
    ImGui::Text("Facing Right: %s", weapon.isFacingRight ? "Yes" : "No");

    ImGui::Separator();
    ImGui::Text("Angles and Rotation");
    ImGui::Separator();

    ImGui::Text("Current Angle: %.2f degrees", weapon.currentAngle);
    ImGui::Text("Starting Angle: %.2f degrees", weapon.initialAngle);
    ImGui::Text("Rotation Speed: %.2f degrees/sec", weapon.rotationSpeed);
    ImGui::Text("Recoil Amount: %.2f", weapon.recoilAmount);

    ImGui::Separator();
    ImGui::Text("Movement and Position");
    ImGui::Separator();

    ImGui::Text("Distance to Travel: %.2f units", weapon.remainingDistance);
    ImGui::Text("Pivot Point: (%d, %d)", weapon.pivotPoint.x, weapon.pivotPoint.y);
    ImGui::Text("Target Point: (%d, %d)", weapon.targetPoint.x, weapon.targetPoint.y);
    ImGui::Text("Target Angle: %.2f degrees", weapon.targetAngleDegrees);

    ImGui::End();
}

void PlayerStatsGUI::displayEntityExplorator(const sf::RenderWindow& window)
{
    // Display the Entity Explorator in the specified position
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window.getSize().x) - 400, 520), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(370, 0), ImGuiCond_FirstUseEver);
    ImGui::Begin("Entity Explorator", nullptr, ImGuiWindowFlags_AlwaysUseWindowPadding);

    ImGui::Separator();
    ImGui::Text("Entity Explorator");
    ImGui::Separator();


    // Create demo tree with 4-5 subtrees
    if (ImGui::TreeNode("Root Entity"))
    {
        if (ImGui::TreeNode("Component 1"))
        {
            ImGui::Text("Detail 1.1");
            ImGui::Text("Detail 1.2");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Component 2"))
        {
            ImGui::Text("Detail 2.1");
            if (ImGui::TreeNode("Sub Component 2.1"))
            {
                ImGui::Text("Detail 2.1.1");
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Component 3"))
        {
            ImGui::Text("Detail 3.1");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Component 4"))
        {
            ImGui::Text("Detail 4.1");
            ImGui::Text("Detail 4.2");
            if (ImGui::TreeNode("Sub Component 4.1"))
            {
                ImGui::Text("Detail 4.1.1");
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Component 5"))
        {
            ImGui::Text("Detail 5.1");
            ImGui::Text("Detail 5.2");
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImGui::End();
}
