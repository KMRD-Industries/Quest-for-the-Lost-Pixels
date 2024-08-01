#include "RenderSystem.h"
#include "AnimationComponent.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "EnemyComponent.h"
#include "EquippedWeaponComponent.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

extern Coordinator gCoordinator;

void RenderSystem::draw(sf::RenderWindow& window)
{
    std::vector<std::vector<sf::Sprite*>> tiles(config::maximumNumberOfLayers);
    const sf::Vector2<unsigned int> windowSize = window.getSize();

    for (const auto entity : m_entities)
    {
        if (auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
            renderComponent.layer > 0 && renderComponent.layer < config::maximumNumberOfLayers)
        {
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
            auto& collisionComponent = gCoordinator.getComponent<ColliderComponent>(entity);

            sf::FloatRect spriteBounds = renderComponent.sprite.getGlobalBounds();
            mapOffset.x = std::max(mapOffset.x, transformComponent.position.x);
            mapOffset.y = std::max(mapOffset.y, transformComponent.position.y);

            // Ensure collision component have valid dimenstions
            if (collisionComponent.collision.height == 0 || collisionComponent.collision.width == 0)
            {
                collisionComponent.collision.height = std::max(spriteBounds.height, config::tileHeight);
                collisionComponent.collision.width = std::max(spriteBounds.width, config::tileHeight);
                collisionComponent.collision.x = {};
                collisionComponent.collision.y = {};
            }

            renderComponent.sprite.setScale(transformComponent.scale * config::gameScale);

            setOrigin(entity);
            setPosition(entity);
            displayDamageTaken(entity);

            tiles[renderComponent.layer].push_back(&gCoordinator.getComponent<RenderComponent>(entity).sprite);
        }
    }

    mapOffset.x = (static_cast<float>(windowSize.x) - mapOffset.x) * 0.5f;
    mapOffset.y = (static_cast<float>(windowSize.y) - mapOffset.y) * 0.5f;

    for (auto& layer : tiles)
    {
        for (const auto& sprite : layer)
        {
            sprite->setPosition(sprite->getPosition() + mapOffset);
            window.draw(*sprite);
        }
    }

    if (config::debugMode)
    {
        debugBoundingBoxes(window);
    }
}

/**
 * \brief Sets the origin of the sprite for the given entity based on its collision component.
 * If the entity has an equipped weapon, also sets the origin for the weapon's sprite.
 * \param entity The entity to process.
 * */
void RenderSystem::setOrigin(const Entity entity) const
{
    if (gCoordinator.hasComponent<WeaponComponent>(entity)) return;

    // Get all necessary components
    auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
    const auto& collisionComponent = gCoordinator.getComponent<ColliderComponent>(entity);

    // Calculate the center of the collision component from top left corner.
    // X & Y are collision offset from top left corner of sprite tile.
    const auto centerX = static_cast<float>(collisionComponent.collision.x + collisionComponent.collision.width / 2);
    const auto centerY = static_cast<float>(collisionComponent.collision.y + collisionComponent.collision.height / 2);

    // Set the origin of the sprite to the center of the collision component
    renderComponent.sprite.setOrigin(centerX, centerY);

    // Handle player related utilities like Equipped Weapon
    if (gCoordinator.hasComponent<EquippedWeaponComponent>(entity))
    {
        const Entity weaponEntity = gCoordinator.getComponent<EquippedWeaponComponent>(entity).currentWeapon;
        const auto& weaponColliderComponent = gCoordinator.getComponent<ColliderComponent>(weaponEntity);
        auto& weaponRenderComponent = gCoordinator.getComponent<RenderComponent>(weaponEntity);

        const auto originX = static_cast<float>(weaponColliderComponent.specialCollision.x);
        const auto originY = static_cast<float>(weaponColliderComponent.specialCollision.y);

        // Set Weapon Sprite origin in WeaponPlacement component.
        weaponRenderComponent.sprite.setOrigin(originX, originY);
    }
}

/**
 * \brief Set up Sprite Position for Render System entity.
 * In addition process all entities connected to entity if possible.
 * \param entity The entity currently processed.
 * */
void RenderSystem::setPosition(const Entity entity) const
{
    if (gCoordinator.hasComponent<WeaponComponent>(entity)) return;

    // Get all necessary components
    const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
    auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

    // Set the position of the Sprite to the position of transform component
    renderComponent.sprite.setPosition(transformComponent.position);

    // Handle displaying player equipment
    if (gCoordinator.hasComponent<EquippedWeaponComponent>(entity))
    {
        // If the entity has an EquippedWeaponComponent, proceed to handle the equipped weapon
        const Entity weaponEntity = gCoordinator.getComponent<EquippedWeaponComponent>(entity).currentWeapon;

        // Get all necessary Components
        const auto& playerColliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
        const auto& weaponColliderComponent = gCoordinator.getComponent<ColliderComponent>(weaponEntity);
        const auto& weaponComponenet = gCoordinator.getComponent<WeaponComponent>(weaponEntity);
        auto& weaponRenderComponent = gCoordinator.getComponent<RenderComponent>(weaponEntity);
        auto& weaponTransformComponent = gCoordinator.getComponent<TransformComponent>(weaponEntity);

        sf::Vector2f weaponPlacement = {};

        // Adjust the weapon's display position based on the relative positions of player and weapon colliders
        // The WeaponPlacement object (object from Tiled) helps in aligning the weapon correctly with the player's
        // sprite
        weaponPlacement.x += static_cast<float>(
            (weaponColliderComponent.specialCollision.x - playerColliderComponent.specialCollision.x) *
            config::gameScale);
        weaponPlacement.y += static_cast<float>(
            (weaponColliderComponent.specialCollision.y - playerColliderComponent.specialCollision.y) *
            config::gameScale);

        sf::Vector2f weaponPosition{};

        // Calculate the weapon's position relative to the player's sprite and game scale
        weaponPosition.x = renderComponent.sprite.getGlobalBounds().left +
            static_cast<float>(weaponColliderComponent.specialCollision.x * config::gameScale);

        weaponPosition.y = renderComponent.sprite.getGlobalBounds().top +
            static_cast<float>(weaponColliderComponent.specialCollision.y * config::gameScale);

        // Update the weapon sprite's position and scale according to the transform component and game scale
        weaponTransformComponent.position = weaponPosition - weaponPlacement;
        weaponTransformComponent.rotation = weaponComponenet.angle;

        weaponRenderComponent.sprite.setPosition(weaponTransformComponent.position);

        if (renderComponent.sprite.getScale().x < 0)
        {
            weaponTransformComponent.rotation -= 60;
        }

        weaponRenderComponent.sprite.setRotation(weaponTransformComponent.rotation);
    }
}

void RenderSystem::displayDamageTaken(const Entity entity) const
{
    if (!gCoordinator.hasComponent<CharacterComponent>(entity)) return;

    auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(entity);
    auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

    if (!characterComponent.attacked)
    {
        renderComponent.sprite.setColor(sf::Color::White);
    }
    else
    {
        renderComponent.sprite.setColor(sf::Color::Red);
        characterComponent.attacked = false;
    }
}

void RenderSystem::debugBoundingBoxes(sf::RenderWindow& window) const
{
    auto renderComponent = gCoordinator.getComponent<RenderComponent>(config::playerEntity);
    auto tileComponent = gCoordinator.getComponent<TileComponent>(config::playerEntity);
    auto transformComponent = gCoordinator.getComponent<TransformComponent>(config::playerEntity);

    auto drawSprite = [&](Entity entity)
    {
        auto renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
        auto tileComponent = gCoordinator.getComponent<TileComponent>(entity);
        const auto bounds = renderComponent.sprite.getGlobalBounds();

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

        sf::CircleShape centerPoint(2);
        centerPoint.setFillColor(sf::Color::Black);
        centerPoint.setPosition(spriteCenter.x, spriteCenter.y);

        convex.setPosition(spriteCenter);

        window.draw(centerPoint);
        window.draw(convex);
    };

    Collision cc =
        gCoordinator.getRegisterSystem<TextureSystem>()->getCollision(tileComponent.tileset, tileComponent.id);

    float xPosCenter = transformComponent.position.x;
    float yPosCenter = transformComponent.position.y;

    const auto center = GameType::MyVec2{xPosCenter + mapOffset.x, yPosCenter + mapOffset.y};

    sf::CircleShape centerPoint(5);
    centerPoint.setFillColor(sf::Color::Red);
    centerPoint.setPosition(center.x, center.y);
    window.draw(centerPoint);

    sf::VertexArray line(sf::Lines, 2);
    line[0].position = center;
    line[0].color = sf::Color::Red;
    line[1].position = {center.x + config::playerAttackRange * transformComponent.scale.x, center.y};
    line[1].color = sf::Color::Red;
    window.draw(line);

    const b2Vec2 newCenter{convertPixelsToMeters(center.x) + mapOffset.x,
                           convertPixelsToMeters(center.y) + mapOffset.y};
    const float newRadius = convertPixelsToMeters(config::playerAttackRange);

    b2CircleShape circle;
    circle.m_radius = newRadius;
    circle.m_p = newCenter;

    for (const auto entity : gCoordinator.getRegisterSystem<CollisionSystem>()->m_entities)
    {
        if (!gCoordinator.hasComponent<ColliderComponent>(entity) ||
            !gCoordinator.hasComponent<TransformComponent>(entity) ||
            !gCoordinator.hasComponent<RenderComponent>(entity))
        {
            continue;
        };

        if (gCoordinator.hasComponent<PlayerComponent>(entity) || gCoordinator.hasComponent<EnemyComponent>(entity) ||
            gCoordinator.hasComponent<WeaponComponent>(entity))
        {
            drawSprite(entity);
        }

        const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);

        if (colliderComponent.body == nullptr)
        {
            continue;
        }

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
                        i, sf::Vector2f(point.x * config::meterToPixelRatio, point.y * config::meterToPixelRatio));
                }
                convex.setFillColor(sf::Color::Transparent);
                convex.setOutlineThickness(1.f);
                convex.setOutlineColor(sf::Color::Green);
                convex.setPosition(colliderComponent.body->GetPosition().x * config::meterToPixelRatio + mapOffset.x,
                                   colliderComponent.body->GetPosition().y * config::meterToPixelRatio + mapOffset.y);

                convex.setRotation(colliderComponent.body->GetAngle() * 180 / b2_pi);
                window.draw(convex);
            }
        }
    }
}
