#include "RenderSystem.h"
#include <DirtyFlagComponent.h>
#include <EnemySystem.h>
#include <EquipmentComponent.h>
#include <SFML/System/Vector2.hpp>
#include <string>
#include "Camera.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "Coordinator.h"
#include "EnemyComponent.h"
#include "GameUtility.h"
#include "Helpers.h"
#include "ItemComponent.h"
#include "MultiplayerComponent.h"
#include "PassageComponent.h"
#include "PlayerComponent.h"
#include "PublicConfigMenager.h"
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

using LayerVertexArray = std::unordered_map<sf::Texture*, sf::VertexArray>;

RenderSystem::RenderSystem()
{
    // Initialize vector of map layers
    m_layeredTextureVertexArrays = std::vector<LayerVertexArray>(configSingleton.GetConfig().maximumNumberOfLayers);
    m_vecSpriteArray = std::vector(configSingleton.GetConfig().maximumNumberOfLayers, std::vector<sf::Sprite*>{});
    m_entityToVertexArrayIndex = std::vector<unsigned long>(MAX_ENTITIES, -1);

    // Load spacial sprites
    portalSprite = gCoordinator.getRegisterSystem<TextureSystem>()->getSprite("portal", 0);

    // Load camera
    camera = Camera{{0.f, 0.f}, {0.f, 0.f}};
}

void RenderSystem::draw(sf::RenderWindow& window)
{
    clearSpriteArray(); // Perform pre-draw cleaning
    std::deque<Entity> entityToRemoveDirtyComponent;

    for (const auto entity : m_entities)
    {
        if (gCoordinator.hasComponent<PlayerComponent>(entity)) players.insert(entity);
        if (gCoordinator.hasComponent<PassageComponent>(entity)) specialObjects.insert(entity);

        auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

        // Static map objects won't be updated frequently, so we will remove their DirtyComponent.
        // This will exclude this entity from consideration in the next for loop,
        // improving performance while still keeping the sprite data in the VertexArrays.
        // To update using updateQuad, a DirtyFlagComponent needs to be added to the entity.

        if (renderComponent.staticMapTile)
        {
            updateQuad(entity);
            entityToRemoveDirtyComponent.push_back(entity);
        }
        else
        {
            updateSprite(entity);
        }
    }

    updateCamera(camera, getPosition(config::playerEntity), window); // Update camera

    // Remove DirtyFlagComponent from static entities
    for (const auto entity : entityToRemoveDirtyComponent)
    {
        gCoordinator.removeComponent<DirtyFlagComponent>(entity);
    }

    // Update special objects rendering (passages etc.)
    for (const auto entity : specialObjects)
    {
        displayPortal(entity);
    }

    // Update specific player related rendering features
    for (const auto player : players)
    {
        updatePlayerSprite(player);
    }

    window.setView(camera.getView());

    // Render layers in order. Newer layers are rendered on top of the previous ones.
    for (int i = 0; i < configSingleton.GetConfig().maximumNumberOfLayers; i++)
    {
        for (const auto& [texture, vertexArray] : m_layeredTextureVertexArrays[i]) window.draw(vertexArray, texture);
        for (const auto& sprite : m_vecSpriteArray[i]) window.draw(*sprite);
    }

    if (configSingleton.GetConfig().debugMode) debugBoundingBoxes(window);
}

/**
 * Update camera to handle displaying map within boundaries.
 * */
void RenderSystem::updateCamera(Camera& camera, const sf::Vector2f targetPos, const sf::RenderTexture& window)
{
    windowSize = window.getSize();
    sf::Vector2f halfView = static_cast<sf::Vector2f>(window.getSize()) * 2.f;

    // Calculate half of map size
    halfView.x = static_cast<float>(windowSize.x) / 2;
    halfView.y = static_cast<float>(windowSize.y) / 2;

    if (GameUtility::mapWidth < windowSize.x && GameUtility::mapHeight < windowSize.y)
    {
        sf::Vector2u mapDimensions;
        mapDimensions.x = static_cast<int>(GameUtility::mapWidth);
        mapDimensions.y = static_cast<int>(GameUtility::mapHeight);

        camera.setSize(windowSize);
        camera.setPosition(static_cast<sf::Vector2f>(mapDimensions) / 2.f);
        return;
    }


    // Clamp the camera position to ensure it stays within the boundaries of the map.
    // This prevents the camera from moving too far to the left or right,
    // without exposing empty space - outsides of map.

    const float clampedX = std::clamp(targetPos.x, halfView.x, GameUtility::mapWidth - halfView.x);
    const float clampedY = std::clamp(targetPos.y, halfView.y, GameUtility::mapHeight - halfView.y);
    camera.setPosition({clampedX, clampedY});

    // camera.setPosition(targetPos);

    // Set camera size to match window size
    camera.setSize(windowSize);
}

/**
 * Clear static map objects
 * */
void RenderSystem::clearMap()
{
    for (auto& layer : m_vecSpriteArray) layer.clear();

    for (auto& layer : m_layeredTextureVertexArrays)
    {
        for (auto& pair : layer) pair.second.clear(); // Clear each VertexArray
        layer.clear();
    }

    specialObjects.clear();

    // Fill with known invalid state
    for (auto& value : m_entityToVertexArrayIndex) value = -1;

    camera.setSize({});
    camera.setPosition({});
    GameUtility::mapHeight = {};
    GameUtility::mapWidth = {};
}

/**
 * Rendering of scene is divided for sf::Sprite and sf::VertexArray
 * sf::VertexArray: Used for rendering static map objects that don't require transformations.
 * VertexArray can render multiple static elements in a single draw call.
 *
 * How this work:
 *  Vertex array is created for each texture - per layer. Then VertexArray is populated with sf::Quad (4 vertices)
 *  for each tile rendered in that layer with that texture. Each sf::Quad consists of position in rendered window and
 *  a rectangle representing part of the texture rendered.
 *
 *  This will make RenderSystem call draw() at MAX_NUMBER_OF_LAYERS * MAX_NUMBER_OF_LAYERS at most to render whole
 *  window instead of calling draw(sprite) for each entity.
 *
 * */
void RenderSystem::updateQuad(const Entity entity)
{
    // All necessary components
    auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
    const auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);
    const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

    // Retrieve the VertexArray for the texture in the specified layer (TileComponent.layer)
    // If it doesn't exist, create a new one of type sf::Quads.
    auto [iterator, inserted] = m_layeredTextureVertexArrays[tileComponent.layer].try_emplace(
        renderComponent.texture, sf::VertexArray(sf::Quads));

    sf::VertexArray* vertexArray = &iterator->second;

    // To speed up computation, render system contains mapping [Entity, vertexArrayIndex].
    // This helps with getting verticals related to each Entity.
    unsigned long vertexArrayIndex{};

    // Check if entity already contains valid vertexes
    if (m_entityToVertexArrayIndex[entity] <= 0 || m_entityToVertexArrayIndex[entity] >= MAX_ENTITIES)
    {
        // Add new mapping and resize VertexArray to make place for new Tile Vertexes
        const size_t currentCount = vertexArray->getVertexCount();
        m_entityToVertexArrayIndex[entity] = currentCount;
        vertexArray->resize(currentCount + 4);
        vertexArrayIndex = currentCount;
    }
    else
    {
        // Get index of first Vertex related with entity
        vertexArrayIndex = m_entityToVertexArrayIndex[entity];
    }

    // To synchronize rendering using VertexArrays with one using Sprites will need texture size.
    const sf::Vector2f texSize = renderComponent.vertexArray[3].texCoords - renderComponent.vertexArray[1].texCoords;
    sf::Vector2f collisionOffset = getOrigin(entity);
    collisionOffset.y *= -1;

    const sf::Vector2f position = getPosition(entity);

    // The map size is described by the bottom-right element's position.
    GameUtility::mapWidth = std::max(GameUtility::mapWidth, position.x);
    GameUtility::mapHeight = std::max(GameUtility::mapHeight, position.y);

    // Ensure vertexArray not null
    if (vertexArray == nullptr) return;

    // Rotation in Vertex Arrays is handled by flipping texture coordinates.
    // For example, a base `sprite` is represented by 4 coordinates in a given texture:
    // [0] = (x_0, y_0 + height),
    // [1] = (x_0 + width, y_0 + height)
    // [2] = (x_0 + width, y_0)
    // [3] = (x_0, y_0)
    //
    // A 90-degree rotation is handled by modifying the indexes of texture coordinates:
    // [(0 + 1)%4] = (x_0, y_0 + height)                -> [0] = (x_0, y_0)
    // [(1 + 1)%4] = (x_0 + width, y_0 + height)        -> [1] = (x_0, y_0 + height),
    // [(2 + 1)%4] = (x_0 + width, y_0)                 -> [2] = (x_0 + width, y_0 + height)
    // [(3 + 1)%4] = (x_0, y_0)                         -> [3] = (x_0 + width, y_0)
    // You get the point - the top-left edge is represented by the top-right in texture, the top-right by the
    // bottom-right. As a result, the displayed texture is rotated.

    const int rotationTexIndexes = static_cast<int>(getRotation(entity) / 90.f);

    for (int i = 0; i < 4; i++)
    {
        // This is to calculate edge position of `sprite` in sf::Texture*
        // [0] = (0, height),
        // [1] = (width, height)
        // [2] = (width, 0)
        // [3] = (0, 0)
        sf::Vector2f offset = renderComponent.vertexArray[i].texCoords - renderComponent.vertexArray[3].texCoords;

        // Since in our game [0,0] is top-left we need adjust texture positioning.
        // In texture coords top-left is represented by [0, height]
        // This code will make sure that [0, 0] will be top-left edge
        offset.y = std::abs(offset.y - texSize.y);

        // Adjust tile position
        sf::Vector2f transformedPos = position + (offset + collisionOffset) * configSingleton.GetConfig().gameScale;

        // Save position, texture, color
        auto& vertex = (*vertexArray)[vertexArrayIndex + i];

        vertex.position = transformedPos + texSize * configSingleton.GetConfig().gameScale;
        vertex.texCoords = renderComponent.vertexArray[(i + rotationTexIndexes) % 4].texCoords;
        vertex.color = renderComponent.color;
    }

    // Similar to rotation handling - vertical and horizontal flip is being made with swapping indexes of VertexArray

    if (transformComponent.scale.x < 0) // Horizontal Flip
    {
        std::swap((*vertexArray)[vertexArrayIndex + 0].texCoords.x, (*vertexArray)[vertexArrayIndex + 1].texCoords.x);
        std::swap((*vertexArray)[vertexArrayIndex + 2].texCoords.x, (*vertexArray)[vertexArrayIndex + 3].texCoords.x);
    }

    if (transformComponent.scale.y < 0) // Vertical Flip
    {
        std::swap((*vertexArray)[vertexArrayIndex + 0].texCoords.y, (*vertexArray)[vertexArrayIndex + 3].texCoords.y);
        std::swap((*vertexArray)[vertexArrayIndex + 1].texCoords.y, (*vertexArray)[vertexArrayIndex + 2].texCoords.y);
    }
}

void RenderSystem::updateSprite(const Entity entity)
{
    // All necessary components
    auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
    auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);

    renderComponent.sprite.setPosition(getPosition(entity));
    renderComponent.sprite.setOrigin(getOrigin(entity));
    renderComponent.sprite.setColor(renderComponent.color);
    renderComponent.sprite.setScale(getScale(entity));
    renderComponent.sprite.setRotation(getRotation(entity));
    renderComponent.color = sf::Color::White;

    displayDamageTaken(entity);

    m_vecSpriteArray[tileComponent.layer].push_back(&renderComponent.sprite);
}

void RenderSystem::clearSpriteArray()
{
    if (m_vecSpriteArray.empty()) m_vecSpriteArray.resize(configSingleton.GetConfig().maximumNumberOfLayers);
    for (auto& layer : m_vecSpriteArray) layer.clear();

    players.clear();
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

        itemTransformComponent.position = itemPosition - itemOrigin;
        itemTransformComponent.rotation = itemRotation;
        itemTransformComponent.centerOfMass = itemOrigin;

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
    const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

    if (gCoordinator.hasComponent<PassageComponent>(entity) && !gCoordinator.hasComponent<PlayerComponent>(entity))
    {
        if (!gCoordinator.getComponent<PassageComponent>(entity).activePassage) return;

        sf::Vector2f portalPosition = {};
        portalPosition.x = transformComponent.position.x -
            portalSprite.getGlobalBounds().width * configSingleton.GetConfig().gameScale / 2;
        portalPosition.y = transformComponent.position.y -
            portalSprite.getGlobalBounds().width * configSingleton.GetConfig().gameScale / 2;
        ;

        sf::Sprite portalSpriteCopy = portalSprite;

        portalSpriteCopy.setPosition(portalPosition);
        portalSpriteCopy.setScale(configSingleton.GetConfig().gameScale, configSingleton.GetConfig().gameScale);
        m_vecSpriteArray[5].push_back(new sf::Sprite(portalSpriteCopy));
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
    const auto& equipment = gCoordinator.getComponent<EquipmentComponent>(entity);
    if (!equipment.slots.contains(GameType::slotType::WEAPON)) return;

    const auto& weapon = gCoordinator.getComponent<WeaponComponent>(equipment.slots.at(GameType::slotType::WEAPON));

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
    if (!gCoordinator.hasComponent<RenderComponent>(config::playerEntity)) return;

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
            displayWeaponStatsTable(window, entity);
            displayPlayerStatsTable(window, entity);
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