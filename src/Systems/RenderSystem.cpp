#include "RenderSystem.h"

#include <EnemySystem.h>

#include <string>
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "Coordinator.h"
#include "DoorComponent.h"
#include "EnemyComponent.h"
#include "EquippedWeaponComponent.h"
#include "FloorComponent.h"
#include "GameUtility.h"
#include "InventoryComponent.h"
#include "ItemComponent.h"
#include "MapComponent.h"
#include "MultiplayerComponent.h"
#include "PassageComponent.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "SpawnerComponent.h"
#include "TextTagComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"
#include "TravellingDungeonComponent.h"
#include "WeaponComponent.h"
#include "imgui.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

extern Coordinator gCoordinator;
extern PublicConfigSingleton configSingleton;

RenderSystem::RenderSystem() { init(); }

void RenderSystem::init() { portalSprite = gCoordinator.getRegisterSystem<TextureSystem>()->getTile("portal", 0); }

void RenderSystem::draw(sf::RenderWindow& window)
{
    if (tiles.empty())
        tiles.resize(configSingleton.GetConfig().maximumNumberOfLayers);
    for (auto& layer : tiles)
        layer.clear();

    const sf::Vector2<unsigned int> windowSize = window.getSize();
    const sf::Vector2f oldMapOffset = {GameUtility::mapOffset};

    for (const auto entity : m_entities)
    {
        auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);

        if (renderComponent.layer > 0 && renderComponent.layer < configSingleton.GetConfig().maximumNumberOfLayers)
        {
            GameUtility::mapOffset.x = std::max(GameUtility::mapOffset.x, transformComponent.position.x);
            GameUtility::mapOffset.y = std::max(GameUtility::mapOffset.y, transformComponent.position.y);

            if (renderComponent.dirty)
            {
                renderComponent.sprite.setScale(transformComponent.scale * configSingleton.GetConfig().gameScale);
                renderComponent.sprite.setRotation(transformComponent.rotation);
                setOrigin(entity);
                setSpritePosition(entity);
                displayDamageTaken(entity);
                displayPortal(entity);
            }

            renderComponent.sprite.setColor(renderComponent.color);
            renderComponent.color = sf::Color::White;

            if (tileComponent.tileSet == "SpecialBlocks" && configSingleton.GetConfig().debugMode)
                tiles[renderComponent.layer + 2].emplace_back(&renderComponent.sprite, &renderComponent.dirty);
            else
                tiles[renderComponent.layer].emplace_back(&renderComponent.sprite, &renderComponent.dirty);
        }
    }

    setWeapon();

    GameUtility::mapOffset = (static_cast<sf::Vector2f>(windowSize) - GameUtility::mapOffset) * 0.5f;
    // GameUtility::mapOffset = {};

    for (auto& layer : tiles)
        for (auto& [sprite, isDirty] : layer)
        {
            if (*isDirty == true)
                sprite->setPosition({sprite->getPosition() + GameUtility::mapOffset});
            window.draw(*sprite);
            *isDirty = oldMapOffset != GameUtility::mapOffset;
        }

    if (configSingleton.GetConfig().debugMode)
        debugBoundingBoxes(window);
}

void RenderSystem::setWeapon()
{
    for (const Entity playerEntity : m_entities)
    {
        if (!((gCoordinator.hasComponent<PlayerComponent>(playerEntity) ||
               gCoordinator.hasComponent<MultiplayerComponent>(playerEntity)) &&
              gCoordinator.hasComponent<EquippedWeaponComponent>(playerEntity)))
            continue;

        const Entity weaponEntity =
            gCoordinator.getComponent<EquippedWeaponComponent>(config::playerEntity).currentWeapon;

        const auto& playerRenderComponent = gCoordinator.getComponent<RenderComponent>(config::playerEntity);
        const auto& playerColliderComponent = gCoordinator.getComponent<ColliderComponent>(config::playerEntity);
        auto& weaponColliderComponent = gCoordinator.getComponent<ColliderComponent>(weaponEntity);
        auto& weaponRenderComponent = gCoordinator.getComponent<RenderComponent>(weaponEntity);
        auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(weaponEntity);
        auto& weaponTransformComponent = gCoordinator.getComponent<TransformComponent>(weaponEntity);

        const auto originXWeapon = weaponColliderComponent.specialCollision.x;
        const auto originYWeapon = weaponColliderComponent.specialCollision.y;
        const auto originXPlayer = playerColliderComponent.specialCollision.x;
        const auto originYPlayer = playerColliderComponent.specialCollision.y;

        // Set Weapon Sprite origin in WeaponPlacement component.
        weaponRenderComponent.sprite.setOrigin(originXWeapon, originYWeapon);

        // Adjust the weapon's display position based on the relative positions of player and weapon colliders
        // The WeaponPlacement object (object from Tiled) helps in aligning the weapon correctly with the player's
        // sprite
        sf::Vector2f weaponPlacement = {};
        weaponPlacement.x += (originXWeapon - originXPlayer) * configSingleton.GetConfig().gameScale;
        weaponPlacement.y += (originYWeapon - originYPlayer) * configSingleton.GetConfig().gameScale;

        // Calculate the weapon's position relative to the player's sprite and game scale
        sf::Vector2f weaponPosition{};
        weaponPosition.x =
            playerRenderComponent.sprite.getGlobalBounds().left + originXWeapon * configSingleton.GetConfig().gameScale;
        weaponPosition.y =
            playerRenderComponent.sprite.getGlobalBounds().top + originYWeapon * configSingleton.GetConfig().gameScale;

        // Update the weapon sprite's position and scale according to the transform component and game scale
        weaponTransformComponent.position = weaponPosition - weaponPlacement;
        weaponTransformComponent.rotation = weaponComponent.currentAngle;

        weaponRenderComponent.sprite.setPosition(weaponTransformComponent.position);
        weaponRenderComponent.sprite.setRotation(weaponTransformComponent.rotation);
        weaponRenderComponent.dirty = true;
    }
}

/**
 * \brief Sets the origin of the sprite for the given entity based on its collision component.
 * If the entity has an equipped weapon, it also sets the origin for the weapon's sprite.
 * \param entity The entity to process.
 * */
void RenderSystem::setOrigin(const Entity entity)
{
    if (gCoordinator.hasComponent<WeaponComponent>(entity))
    {
        if (gCoordinator.getComponent<WeaponComponent>(entity).equipped == true)
            return;
    }

    // Get all necessary components
    auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
    float centerX = {}, centerY = {};

    if (const auto* colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity))
    {
        // Calculate the center of the collision component from top left corner.
        // X & Y are collision offset from top left corner of sprite tile.
        centerX = colliderComponent->collision.x + colliderComponent->collision.width / 2;
        centerY = colliderComponent->collision.y + colliderComponent->collision.height / 2;
    }
    else
    {
        centerX = configSingleton.GetConfig().tileHeight / 2;
        centerY = configSingleton.GetConfig().tileHeight / 2;
    }

    renderComponent.sprite.setOrigin(centerX, centerY);
}

/**
 * \brief Set up Sprite Position for Render System entity.
 * In addition, process all entities connected to entity if possible.
 * \param entity The entity currently processed.
 * */
void RenderSystem::setSpritePosition(const Entity entity)
{
    if (gCoordinator.hasComponent<WeaponComponent>(entity))
    {
        if (gCoordinator.getComponent<WeaponComponent>(entity).equipped == true)
            return;
    }

    // Get all necessary parts
    const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
    auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

    // Set the position of the Sprite to the position of transform component
    renderComponent.sprite.setPosition(transformComponent.position);
}

void RenderSystem::displayPortal(const Entity entity)
{
    const auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

    if (gCoordinator.hasComponent<PassageComponent>(entity))
    {
        if (
            !gCoordinator.hasComponent<PlayerComponent>(entity))
        {
            sf::Vector2f portalPosition = {};
            portalPosition.x = renderComponent.sprite.getPosition().x - renderComponent.sprite.getLocalBounds().width -
                9 * configSingleton.GetConfig().gameScale;
            portalPosition.y = renderComponent.sprite.getPosition().y - renderComponent.sprite.getLocalBounds().height -
                8 * configSingleton.GetConfig().gameScale;

            sf::Sprite portalSpriteCopy = portalSprite;

            portalSpriteCopy.setPosition(portalPosition);
            portalSpriteCopy.setScale(renderComponent.sprite.getScale());

            tiles[3].emplace_back(new sf::Sprite(portalSpriteCopy), new bool(true));
        }
    }
}

void RenderSystem::displayDamageTaken(const Entity entity)
{
    if (!gCoordinator.hasComponent<CharacterComponent>(entity))
        return;

    auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(entity);
    auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

    if (!characterComponent.attacked)
    {
        if (!gCoordinator.hasComponent<WeaponComponent>(entity))
            renderComponent.sprite.setColor(sf::Color::White);
    }
    else
    {
        characterComponent.timeSinceAttacked++;
        renderComponent.sprite.setColor(sf::Color::Red);
        if (characterComponent.timeSinceAttacked >= 5)
        {
            characterComponent.attacked = false;
            characterComponent.timeSinceAttacked = 0;
        }
    }
}

void RenderSystem::displayPlayerStatsTable(const sf::RenderWindow& window, const Entity entity) const
{
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window.getSize().x) - 300, 370), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(270, 0), ImGuiCond_Always); // Set the width to 250, height is auto
    ImGui::Begin("Player Stats", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysUseWindowPadding);

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

void RenderSystem::displayWeaponStatsTable(const sf::RenderWindow& window, const Entity entity)
{
    const auto& weaponComponent = gCoordinator.getComponent<EquippedWeaponComponent>(entity);
    const auto& weapon = gCoordinator.getComponent<WeaponComponent>(weaponComponent.currentWeapon);

    // Display the Weapon Stats table in the top-right corner
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window.getSize().x) - 300, 10));
    ImGui::SetNextWindowSize(ImVec2(270, 0));
    ImGui::Begin("Weapon Stats", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysUseWindowPadding);

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


void RenderSystem::debugBoundingBoxes(sf::RenderWindow& window)
{
    if (!gCoordinator.hasComponent<RenderComponent>(config::playerEntity))
        return;

    auto renderComponent = gCoordinator.getComponent<RenderComponent>(config::playerEntity);
    auto tileComponent = gCoordinator.getComponent<TileComponent>(config::playerEntity);
    auto transformComponent = gCoordinator.getComponent<TransformComponent>(config::playerEntity);

    auto drawSprite = [&](const Entity entity)
    {
        const auto& rComponent = gCoordinator.getComponent<RenderComponent>(entity);
        const auto bounds = rComponent.sprite.getGlobalBounds();

        sf::ConvexShape convex;
        convex.setPointCount(4);
        convex.setPoint(0, {0, 0});
        convex.setPoint(1, {bounds.width, 0});
        convex.setPoint(2, {bounds.width, bounds.height});
        convex.setPoint(3, {0, bounds.height});

        convex.setFillColor(sf::Color::Transparent);
        convex.setOutlineThickness(1.f);
        convex.setOutlineColor(sf::Color::Yellow);

        const auto spriteCenter = GameType::MyVec2{bounds.left, bounds.top};
        convex.setPosition(spriteCenter);

        window.draw(convex);
    };

    Collision cc =
        gCoordinator.getRegisterSystem<TextureSystem>()->getCollision(tileComponent.tileSet, tileComponent.id);

    // const auto center = GameType::MyVec2{xPosCenter + m_mapOffset.x, yPosCenter + m_mapOffset.y};
    const auto center = GameType::MyVec2{transformComponent.position + GameUtility::mapOffset};

    sf::CircleShape centerPoint(5);
    centerPoint.setFillColor(sf::Color::Red);
    centerPoint.setPosition(center.x, center.y);
    window.draw(centerPoint);

    const b2Vec2 newCenter{convertPixelsToMeters(center) + GameUtility::mapOffset};
    const float newRadius = convertPixelsToMeters(configSingleton.GetConfig().playerAttackRange);

    b2CircleShape circle;
    circle.m_radius = newRadius;
    circle.m_p = newCenter;

    for (const auto entity : gCoordinator.getRegisterSystem<CollisionSystem>()->m_entities)
    {
        if (!gCoordinator.hasComponent<ColliderComponent>(entity) ||
            !gCoordinator.hasComponent<TransformComponent>(entity) ||
            !gCoordinator.hasComponent<RenderComponent>(entity))
            continue;

        if (gCoordinator.hasComponent<PlayerComponent>(entity) ||
            gCoordinator.hasComponent<MultiplayerComponent>(entity) ||
            gCoordinator.hasComponent<EnemyComponent>(entity) || gCoordinator.hasComponent<WeaponComponent>(entity))
            drawSprite(entity);

        if (gCoordinator.hasComponent<EquippedWeaponComponent>(entity))
        {
            displayWeaponStatsTable(window, entity);
            displayPlayerStatsTable(window, entity);
        }

        if ((gCoordinator.hasComponent<PlayerComponent>(entity) ||
             gCoordinator.hasComponent<MultiplayerComponent>(entity)) &&
            gCoordinator.hasComponent<EquippedWeaponComponent>(entity))
        {
            const auto& weaponComponent = gCoordinator.getComponent<EquippedWeaponComponent>(entity);
            const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

            sf::VertexArray swordLine(sf::Lines, 2);
            swordLine[0].position = transformComponent.position + GameUtility::mapOffset;
            swordLine[0].color = sf::Color::Red;
            swordLine[1].position =
                gCoordinator.getComponent<WeaponComponent>(weaponComponent.currentWeapon).pivotPoint;
            swordLine[1].color = sf::Color::Blue;
            window.draw(swordLine);
        }


        const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);

        if (colliderComponent.body == nullptr)
            continue;

        for (b2Fixture* fixture = colliderComponent.body->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            b2Shape* shape = fixture->GetShape();
            if (shape->GetType() == b2Shape::e_polygon)
            {
                auto* const polygonShape = dynamic_cast<b2PolygonShape*>(shape);
                const int32 count = polygonShape->m_count;
                sf::ConvexShape convex;
                convex.setPointCount(count);
                for (int32 i = 0; i < count; ++i)
                {
                    const b2Vec2 point = polygonShape->m_vertices[i];
                    convex.setPoint(
                        i, sf::Vector2f(point.x * configSingleton.GetConfig().meterToPixelRatio,
                                        point.y * configSingleton.GetConfig().meterToPixelRatio));
                }
                convex.setFillColor(sf::Color::Transparent);
                convex.setOutlineThickness(1.f);
                convex.setOutlineColor(sf::Color::Green);
                convex.setPosition(
                    colliderComponent.body->GetPosition().x * static_cast<float>(configSingleton.GetConfig().
                        meterToPixelRatio) +
                    GameUtility::mapOffset.x,
                    colliderComponent.body->GetPosition().y * static_cast<float>(configSingleton.GetConfig().
                        meterToPixelRatio) +
                    GameUtility::mapOffset.y);

                convex.setRotation(colliderComponent.body->GetAngle() * 180 / b2_pi);
                window.draw(convex);
            }
        }
    }
}
