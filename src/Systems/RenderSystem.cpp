#include "RenderSystem.h"
#include <EnemySystem.h>
#include <EquipmentComponent.h>
#include <string>
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "Coordinator.h"
#include "EnemyComponent.h"
#include "GameUtility.h"
#include "ItemComponent.h"
#include "MultiplayerComponent.h"
#include "PassageComponent.h"
#include "PlayerComponent.h"
#include "PlayerStatsGUI.h"
#include "RenderComponent.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "TextTagComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "imgui.h"

extern Coordinator gCoordinator;
extern PublicConfigSingleton configSingleton;

RenderSystem::RenderSystem() { init(); }

void RenderSystem::init()
{
    portalSprite = gCoordinator.getRegisterSystem<TextureSystem>()->getTile("portal", 0);
    m_playerStatsGui = std::make_unique<PlayerStatsGUI>().get();
}

void RenderSystem::draw(sf::RenderWindow& window)
{
    clear(window);
    const sf::Vector2f oldMapOffset = {GameUtility::mapOffset};

    for (const auto entity : m_entities)
    {
        if (gCoordinator.hasComponent<PlayerComponent>(entity)) players.push_back(entity);

        auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        GameUtility::mapOffset.x = std::max(GameUtility::mapOffset.x, transformComponent.position.x);
        GameUtility::mapOffset.y = std::max(GameUtility::mapOffset.y, transformComponent.position.y);

        if (renderComponent.dirty) updateSprite(entity);
    }

    // Update player related sprites
    for (const auto player : players)
    {
        updatePlayerSprite(player);
    }

    GameUtility::mapOffset = (static_cast<sf::Vector2f>(windowSize) - GameUtility::mapOffset) * 0.5f;

    for (auto& layer : tiles)
        for (auto& [sprite, isDirty] : layer)
        {
            if (*isDirty == true) sprite->setPosition({sprite->getPosition() + GameUtility::mapOffset});
            window.draw(*sprite);
            *isDirty = oldMapOffset != GameUtility::mapOffset;
        }

    if (configSingleton.GetConfig().debugMode) debugBoundingBoxes(window);
}

void RenderSystem::updateSprite(const Entity entity)
{
    auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
    const auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);

    // Set sprite
    renderComponent.sprite.setPosition(getPosition(entity));
    renderComponent.sprite.setOrigin(getOrigin(entity));
    renderComponent.sprite.setColor(renderComponent.color);
    renderComponent.sprite.setScale(getScale(entity));
    renderComponent.sprite.setRotation(getRotation(entity));

    renderComponent.color = sf::Color::White;

    displayDamageTaken(entity);
    displayPortal(entity);

    if (gCoordinator.hasComponent<ItemComponent>(entity))
        if (gCoordinator.getComponent<ItemComponent>(entity).equipped == false) renderComponent.layer = 4;

    if (tileComponent.tileSet == "SpecialBlocks" && configSingleton.GetConfig().debugMode)
        tiles[renderComponent.layer + 2].emplace_back(&renderComponent.sprite, &renderComponent.dirty);
    else
        tiles[renderComponent.layer].emplace_back(&renderComponent.sprite, &renderComponent.dirty);
}

void RenderSystem::clear(const sf::Window& window)
{
    if (tiles.empty()) tiles.resize(configSingleton.GetConfig().maximumNumberOfLayers);
    for (auto& layer : tiles) layer.clear();
    players.clear();
    windowSize = window.getSize();
    newMapOffset = {};
}

void RenderSystem::updatePlayerSprite(const Entity entity) { setEquipment(entity); }

void RenderSystem::setEquipment(const Entity entity)
{
    const auto& [equipment] = gCoordinator.getComponent<EquipmentComponent>(config::playerEntity);

    auto& playerRenderComponent = gCoordinator.getComponent<RenderComponent>(entity);
    auto& playerColliderComponent = gCoordinator.getComponent<ColliderComponent>(config::playerEntity);

    for (const auto it : equipment)
    {
        auto& itemRenderComponent = gCoordinator.getComponent<RenderComponent>(it.second);
        auto& itemCollisionComponent = gCoordinator.getComponent<ColliderComponent>(it.second);
        auto& itemTransformComponent = gCoordinator.getComponent<TransformComponent>(it.second);

        const bool reversed = playerRenderComponent.sprite.getScale().x < 0;

        Collision* itemPivot{nullptr};
        Collision* itemPlacement{nullptr};

        switch (it.first)
        {
        case GameType::slotType::BODY_ARMOUR:
            {
                itemPlacement = &itemCollisionComponent.bodyArmourPlacement;
                itemPivot = &playerColliderComponent.bodyArmourPlacement;
                break;
            }
        case GameType::slotType::WEAPON:
            {
                itemPlacement = &itemCollisionComponent.weaponPlacement;
                itemPivot = &playerColliderComponent.weaponPlacement;
                break;
            }
        case GameType::slotType::HELMET:
            {
                itemPlacement = &itemCollisionComponent.helmetPlacement;
                itemPivot = &playerColliderComponent.helmetPlacement;
                break;
            }
        }

        if (!(itemPivot && itemPlacement)) return;
        if (reversed) reverseDisplay(*itemPivot, *itemPlacement);

        auto itemOrigin = getEquippedItemOrigin(*itemPlacement);
        auto itemPosition = getEquippedItemPosition(*itemPivot, *itemPlacement, playerRenderComponent);
        const float itemRotation = getEquippedItemRotation(it.second);

        itemTransformComponent.position = itemPosition;
        itemTransformComponent.rotation = itemRotation;

        itemRenderComponent.sprite.setOrigin(itemOrigin);
        itemRenderComponent.sprite.setPosition(itemPosition);
        itemRenderComponent.sprite.setRotation(itemRotation);
    }
}

sf::Vector2f RenderSystem::getEquippedItemOrigin(Collision& itemPlacement)
{
    return {itemPlacement.x, itemPlacement.y};
}

sf::Vector2f RenderSystem::getEquippedItemPosition(const Collision& itemPivot, const Collision& itemPlacement,
                                                   const RenderComponent& playerSprite)
{
    sf::Vector2f weaponPlacement = {};
    weaponPlacement.x += (itemPlacement.x - itemPivot.x) * configSingleton.GetConfig().gameScale;
    weaponPlacement.y += (itemPlacement.y - itemPivot.y) * configSingleton.GetConfig().gameScale;

    // Calculate the weapon's position relative to the player's sprite and game scale
    sf::Vector2f weaponPosition{};
    weaponPosition.x =
        playerSprite.sprite.getGlobalBounds().left + itemPlacement.x * configSingleton.GetConfig().gameScale;
    weaponPosition.y =
        playerSprite.sprite.getGlobalBounds().top + itemPlacement.y * configSingleton.GetConfig().gameScale;

    // Update the weapon sprite's position and scale according to the transform component and game scale
    return weaponPosition - weaponPlacement;
}

float RenderSystem::getEquippedItemRotation(const Entity entity)
{
    if (const auto* weaponComponent = gCoordinator.tryGetComponent<WeaponComponent>(entity))
        return weaponComponent->currentAngle;

    return 0.f;
}

void RenderSystem::reverseDisplay(Collision& itemPivot, Collision& itemPlacement)
{
    itemPlacement.x = configSingleton.GetConfig().tileHeight - itemPlacement.x - itemPlacement.width;
    itemPivot.x = configSingleton.GetConfig().tileHeight - itemPivot.x - itemPivot.width;
}

/**
 * \brief Sets the origin of the sprite for the given entity based on its collision component.
 * If the entity has an equipped weapon, it also sets the origin for the weapon's sprite.
 * \param entity The entity to process.
 * */
sf::Vector2f RenderSystem::getOrigin(const Entity entity)
{
    sf::Vector2f originPosition{};

    if (const auto* colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity))
    {
        // Calculate the center of the collision component from top left corner.
        // X & Y are collision offset from top left corner of sprite tile.
        originPosition.x = colliderComponent->collision.x + colliderComponent->collision.width / 2;
        originPosition.y = colliderComponent->collision.y + colliderComponent->collision.height / 2;

        return originPosition;
    }

    return {configSingleton.GetConfig().tileHeight / 2, configSingleton.GetConfig().tileHeight / 2};
}

sf::Vector2f RenderSystem::getPosition(const Entity entity)
{
    if (const auto* transformComponent = gCoordinator.tryGetComponent<TransformComponent>(entity))
        return transformComponent->position;

    return {};
}

sf::Vector2f RenderSystem::getScale(const Entity entity)
{
    if (const auto* transformComponent = gCoordinator.tryGetComponent<TransformComponent>(entity))
        return transformComponent->scale * configSingleton.GetConfig().gameScale;

    return {};
}

float RenderSystem::getRotation(const Entity entity)
{
    if (const auto* transformComponent = gCoordinator.tryGetComponent<TransformComponent>(entity))
        return transformComponent->rotation;

    return {};
}

void RenderSystem::displayPortal(const Entity entity)
{
    const auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

    if (gCoordinator.hasComponent<PassageComponent>(entity) && !gCoordinator.hasComponent<PlayerComponent>(entity))
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

void RenderSystem::displayDamageTaken(const Entity entity)
{
    if (!gCoordinator.hasComponent<CharacterComponent>(entity)) return;

    auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(entity);
    auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

    if (!characterComponent.attacked)
    {
        if (!gCoordinator.hasComponent<WeaponComponent>(entity)) renderComponent.sprite.setColor(sf::Color::White);
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

void RenderSystem::debugBoundingBoxes(sf::RenderWindow& window)
{
    if (!gCoordinator.hasComponent<RenderComponent>(config::playerEntity)) return;

    m_playerStatsGui->displayEntityExplorator(window);

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

        if (gCoordinator.hasComponent<EquipmentComponent>(entity))
        {
            m_playerStatsGui->displayPlayerStatsTable(window, entity);
            m_playerStatsGui->displayWeaponStatsTable(window, entity);
        }

        if (gCoordinator.hasComponent<PlayerComponent>(entity) && gCoordinator.hasComponent<EquipmentComponent>(entity))
        {
            auto& weaponComponent = gCoordinator.getComponent<EquipmentComponent>(entity);

            sf::VertexArray swordLine(sf::Lines, 2);
            swordLine[0].position = center;
            swordLine[0].color = sf::Color::Red;
            swordLine[1].position =
                gCoordinator.getComponent<WeaponComponent>(weaponComponent.slots.at(GameType::slotType::WEAPON))
                    .pivotPoint;
            swordLine[1].color = sf::Color::Blue;
            window.draw(swordLine);
        }

        const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);

        if (colliderComponent.body == nullptr) continue;

        for (b2Fixture* fixture = colliderComponent.body->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            if (fixture->GetShape() == nullptr) return;

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
                    convex.setPoint(i,
                                    sf::Vector2f(point.x * configSingleton.GetConfig().meterToPixelRatio,
                                                 point.y * configSingleton.GetConfig().meterToPixelRatio));
                }
                convex.setFillColor(sf::Color::Transparent);
                convex.setOutlineThickness(1.f);
                convex.setOutlineColor(sf::Color::Green);
                convex.setPosition(colliderComponent.body->GetPosition().x *
                                           static_cast<float>(configSingleton.GetConfig().meterToPixelRatio) +
                                       GameUtility::mapOffset.x,
                                   colliderComponent.body->GetPosition().y *
                                           static_cast<float>(configSingleton.GetConfig().meterToPixelRatio) +
                                       GameUtility::mapOffset.y);

                convex.setRotation(colliderComponent.body->GetAngle() * 180 / b2_pi);
                window.draw(convex);
            }
        }
    }
}