#include <chrono>
#include <cstdint>
#include <iostream>

#include <boost/asio.hpp>
#include <comm.pb.h>
#include <vector>
#include <zlib.h>

#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "Config.h"
#include "Coordinator.h"
#include "EquipmentComponent.h"
#include "FightActionEvent.h"
#include "GameTypes.h"
#include "Helpers.h"
#include "InputHandler.h"
#include "InventorySystem.h"
#include "MultiplayerSystem.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "RoomListenerSystem.h"
#include "SpawnerSystem.h"
#include "SynchronisedEvent.h"
#include "TransformComponent.h"
#include "Types.h"
#include "WeaponComponent.h"

extern Coordinator gCoordinator;

constexpr uint32_t DIFF = 4096;

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

using sysClock = std::chrono::system_clock;

inline bool readyToTick(sysClock::time_point older, sysClock::time_point newer)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(newer - older).count() > config::millisPerTick;
}

void MultiplayerSystem::setup(const std::string_view& ip, const std::string_view& port) noexcept
{
    try
    {
        boost::asio::steady_timer timer(m_io_context);

        timer.expires_after(std::chrono::seconds(5));
        udp::resolver udp_resolver(m_io_context);
        udp::resolver::results_type udp_endpoints = udp_resolver.resolve(ip, port);

        tcp::resolver tcp_resolver(m_io_context);
        tcp::resolver::results_type tcp_endpoints = tcp_resolver.resolve(ip, port);

        std::cout << ip << " " << port << "\n";

        m_udp_socket.connect(*udp_endpoints.begin());
        m_tcp_socket.async_connect(*tcp_endpoints.begin(),
                                   [&](const boost::system::error_code& ec)
                                   {
                                       timer.cancel();
                                       if (!ec) m_connected = true;
                                   });

        timer.async_wait(
            [&](const boost::system::error_code& ec)
            {
                if (!m_connected && !ec) m_tcp_socket.cancel();
            });

        m_io_context.run();
    }
    catch (boost::system::system_error)
    {
        return;
    }

    if (!m_connected) return;

    m_last_tick = sysClock::now();

    comm::BytePrefix prefixDummy;
    prefixDummy.set_bytes(DIFF);
    m_prefix_size = static_cast<int>(prefixDummy.ByteSizeLong());
    m_prefix_buf.resize(m_prefix_size);

    m_updates.mutable_updates()->Reserve(64);
}

void MultiplayerSystem::setRoom(const glm::ivec2& room) noexcept
{
    m_current_room = room;

    auto r1 = m_state.mutable_room();
    r1->set_x(room.x);
    r1->set_y(room.y);

    auto r2 = m_outgoing_movement.mutable_curr_room();
    r2->set_x(room.x);
    r2->set_y(room.y);
}

const glm::ivec2& MultiplayerSystem::getRoom() const noexcept { return m_current_room; }

const ItemGenerator& MultiplayerSystem::getItemGenerator() { return m_item_generator; }

bool MultiplayerSystem::isInsideInitialRoom(const bool change) noexcept
{
    bool ret = m_inside_initial_room;
    if (change) m_inside_initial_room = false;
    return ret;
}

bool MultiplayerSystem::isConnected() const noexcept { return m_connected; }
void MultiplayerSystem::setPlayer(const uint32_t id, const Entity entity) { m_entity_map[id] = entity; }
uint32_t MultiplayerSystem::playerID() const noexcept { return m_player_id; }

comm::InitialInfo MultiplayerSystem::registerPlayer(const Entity playerEntity)
{
    size_t received = m_tcp_socket.receive(boost::asio::buffer(m_buf));

    comm::InitialInfo initialInfo;
    initialInfo.ParseFromArray(&m_buf, static_cast<int>(received));

    uint32_t id = initialInfo.player().id();
    m_entity_map[id] = playerEntity;
    m_player_entity = playerEntity;
    m_player_id = id;

    m_seed = initialInfo.seed();

    const auto& nextItem = initialInfo.next_item();
    m_item_generator.id = nextItem.id();
    m_item_generator.gen = nextItem.gen();
    m_item_generator.type = nextItem.type();
    m_generator_ready = true;

    return initialInfo;
}

void MultiplayerSystem::pollState()
{
    size_t available = m_tcp_socket.available();
    if (available >= m_prefix_size)
    {
        // read message byte size
        size_t received = m_tcp_socket.read_some(boost::asio::buffer(m_prefix_buf, m_prefix_size));
        m_prefix.ParseFromArray(m_prefix_buf.data(), m_prefix_size);

        uint32_t msg_size = m_prefix.bytes() - DIFF;

        available = m_tcp_socket.available();
        while (available < msg_size) available = m_tcp_socket.available();

        // read exactly msg_size bytes
        received = m_tcp_socket.read_some(boost::asio::buffer(m_buf, msg_size));

        m_state.ParseFromArray(m_buf.data(), static_cast<int>(received));
        std::cout << m_state.ShortDebugString() << '\n';

        const auto& player = m_state.player();
        const auto& item = m_state.item();
        const auto& room = m_state.room();

        const uint32_t playerID = player.id();

        switch (m_state.variant())
        {
        case comm::CONNECTED:
            {
                auto dungeonUpdate = MultiplayerDungeonUpdate{
                    .variant = MultiplayerDungeonUpdate::Variant::REGISTER_PLAYER, .player = player};
                m_dungeon_updates.push_back(dungeonUpdate);
                break;
            }
        case comm::PLAYER_DIED:
        case comm::DISCONNECTED:
            if (playerID == 0 || !m_entity_map.contains(playerID)) break;

            gCoordinator.getComponent<ColliderComponent>(m_entity_map[playerID]).toDestroy = true;
            for (auto& slot : gCoordinator.getComponent<EquipmentComponent>(m_entity_map[playerID]).slots)
            {
                if (slot.second != 0)
                {
                    m_registered_items.erase(slot.second);
                    gCoordinator.getComponent<ColliderComponent>(slot.second).toDestroy = true;
                }
            }
            m_entity_map.erase(playerID);
            break;
        case comm::REQUEST_ITEM_GENERATOR:
            m_generator_ready = true;
            m_item_generator.id = item.id();
            m_item_generator.gen = item.gen();
            m_item_generator.type = item.type();
            break;
        case comm::ITEM_EQUIPPED:
            switch (item.type())
            {
            case comm::WEAPON:
                gCoordinator.getRegisterSystem<InventorySystem>()->pickUpItem(
                    GameType::PickUpInfo{m_entity_map[playerID], getItemEntity(item.id()), GameType::slotType::WEAPON});
                break;
            case comm::HELMET:
                gCoordinator.getRegisterSystem<InventorySystem>()->pickUpItem(
                    GameType::PickUpInfo{m_entity_map[playerID], getItemEntity(item.id()), GameType::slotType::HELMET});
                break;
            case comm::ARMOUR:
                gCoordinator.getRegisterSystem<InventorySystem>()->pickUpItem(GameType::PickUpInfo{
                    m_entity_map[playerID], getItemEntity(item.id()), GameType::slotType::BODY_ARMOUR});
                break;
            default:
                break;
            }
            break;
        case comm::ROOM_CHANGED:
            {
                setRoom({room.x(), room.y()});
                auto dungeonUpdate = MultiplayerDungeonUpdate{.variant = MultiplayerDungeonUpdate::Variant::CHANGE_ROOM,
                                                              .room = m_current_room};
                m_dungeon_updates.push_back(dungeonUpdate);
                m_isMapDimensionsSent = false;
                m_areSpawnersSent = false;
                break;
            }
        case comm::ROOM_CLEARED:
            gCoordinator.getRegisterSystem<RoomListenerSystem>()->spawnLoot();
            break;
        case comm::LEVEL_CHANGED:
            {
                auto dungeonUpdate =
                    MultiplayerDungeonUpdate{.variant = MultiplayerDungeonUpdate::Variant::CHANGE_LEVEL};
                m_dungeon_updates.push_back(dungeonUpdate);
                break;
            }
        case comm::SPAWN_ENEMY_REQUEST:
            gCoordinator.getRegisterSystem<SpawnerSystem>()->spawnOnDemand(m_state);
            break;
        default:
            {
            }
        }
    }
    else
    {
        m_state.set_variant(comm::NONE);
    }
}

Entity MultiplayerSystem::getItemEntity(const uint32_t id)
{
    if (m_registered_items.contains(id)) return m_registered_items[id];
    return 0;
}

int64_t MultiplayerSystem::getSeed() { return m_seed; }

const std::unordered_map<uint32_t, Entity>& MultiplayerSystem::getPlayers() { return m_entity_map; }

const std::vector<MultiplayerDungeonUpdate>& MultiplayerSystem::getRemoteDungeonUpdates() { return m_dungeon_updates; }
void MultiplayerSystem::clearRemoteDungeonUpdates()
{
    if (m_dungeon_updates.size() > 0) m_dungeon_updates.clear();
}

void MultiplayerSystem::update(const float deltaTime)
{
    if (!m_connected)
    {
        std::deque<Entity> eventEntities;

        for (const Entity entity : m_entities) eventEntities.push_back(entity);

        while (!eventEntities.empty())
        {
            gCoordinator.destroyEntity(eventEntities.front());
            eventEntities.pop_front();
        }
        return;
    }

    if (m_frameTime += deltaTime; m_frameTime < configSingleton.GetConfig().oneFrameTime * 1000) return;

    m_frameTime -= configSingleton.GetConfig().oneFrameTime * 1000;

    pollState();
    pollMovement();

    std::vector<Entity> synchronisedEvents{};
    std::vector<Entity> movementEvents{};
    for (const Entity eventEntity : m_entities)
    {
        const auto& eventComponent = gCoordinator.getComponent<SynchronisedEvent>(eventEntity);
        switch (eventComponent.updateType)
        {
        case SynchronisedEvent::UpdateType::STATE:
            synchronisedEvents.push_back(eventEntity);
            break;
        case SynchronisedEvent::UpdateType::MOVEMENT:
            movementEvents.push_back(eventEntity);
        default:
            {
            }
        }
    }

    if (synchronisedEvents.size() > 0) updateState(synchronisedEvents);
    if (movementEvents.size() > 0) updateMovement(movementEvents);

    std::deque<Entity> eventEntities;

    for (const Entity entity : m_entities) eventEntities.push_back(entity);

    while (!eventEntities.empty())
    {
        gCoordinator.destroyEntity(eventEntities.front());
        eventEntities.pop_front();
    }

    if (m_areSpawnersSent)
    {
        gatherEnemyAndPlayerPositions();
        updateMap();
    }
}

void MultiplayerSystem::updateState(const std::vector<Entity>& entities)
{
    comm::StateUpdate* update = nullptr;
    bool anythingToSend = false, usedPreviousUpdate = true;

    m_updates.mutable_updates()->Clear();

    for (const Entity eventEntity : entities)
    {
        const auto& [updateType, variant, entityID, entity, updatedEntity, pickUpInfo, room, obstacleData] =
            gCoordinator.getComponent<SynchronisedEvent>(eventEntity);

        if (usedPreviousUpdate) update = m_updates.add_updates();

        switch (variant)
        {
        case SynchronisedEvent::Variant::PLAYER_CONNECTED:
            usedPreviousUpdate = false;
            if (entity == 0 || entityID == 0) continue;

            m_entity_map[entityID] = entity;
            break;

        case SynchronisedEvent::Variant::PLAYER_KILLED:
            usedPreviousUpdate = true;
            anythingToSend = true;

            m_alive = false;

            for (auto& slot : gCoordinator.getComponent<EquipmentComponent>(entity).slots)
            {
                if (slot.second != 0)
                {
                    m_registered_items.erase(slot.second);
                    if (auto cc = gCoordinator.tryGetComponent<ColliderComponent>(slot.second)) cc->toDestroy = true;
                }
            }

            gCoordinator.removeComponent<RenderComponent>(entity);
            gCoordinator.removeComponent<PlayerComponent>(entity);
            gCoordinator.getComponent<ColliderComponent>(entity).toRemoveCollider = true;

            update->set_variant(comm::PLAYER_DIED);
            update->mutable_player()->set_id(m_player_id);
            break;

        case SynchronisedEvent::Variant::REGISTER_ITEM:
            usedPreviousUpdate = false;
            if (entity == 0 || entityID == 0) continue;

            m_registered_items[entityID] = entity;
            break;

        case SynchronisedEvent::Variant::REQUEST_ITEM_GENERATOR:
            usedPreviousUpdate = true;
            anythingToSend = true;

            update->set_variant(comm::REQUEST_ITEM_GENERATOR);
            update->mutable_player()->set_id(m_player_id);
            break;

        case SynchronisedEvent::Variant::UPDATE_ITEM_ENTITY:
            usedPreviousUpdate = false;
            if (entity == 0 || updatedEntity == 0) continue;

            for (const auto& p : m_registered_items)
            {
                if (p.second != entity) continue;

                m_registered_items[p.first] = updatedEntity;
                break;
            }
            break;

        case SynchronisedEvent::Variant::ITEM_EQUIPPED:
            usedPreviousUpdate = true;
            anythingToSend = true;

            if (!pickUpInfo.has_value())
            {
                usedPreviousUpdate = false;
                continue;
            }

            for (const auto& p : m_registered_items)
            {
                if (p.second != pickUpInfo->itemEntity) continue;

                update->set_variant(comm::ITEM_EQUIPPED);
                update->mutable_player()->set_id(m_player_id);

                auto item = update->mutable_item();
                item->set_id(p.first);

                switch (pickUpInfo->slot)
                {
                case GameType::slotType::WEAPON:
                    item->set_type(comm::WEAPON);
                    break;
                case GameType::slotType::HELMET:
                    item->set_type(comm::HELMET);
                    break;
                case GameType::slotType::BODY_ARMOUR:
                    item->set_type(comm::ARMOUR);
                    break;
                default:
                    break;
                }
                break;
            }
            break;

        case SynchronisedEvent::Variant::ROOM_CHANGED:
            usedPreviousUpdate = true;
            anythingToSend = true;

            if (!room.has_value())
            {
                usedPreviousUpdate = false;
                continue;
            }

            setRoom(*room);
            update->set_variant(comm::ROOM_CHANGED);
            update->mutable_room()->set_x(room->x);
            update->mutable_room()->set_y(room->y);
            break;
        case SynchronisedEvent::Variant::ROOM_CLEARED:
            usedPreviousUpdate = true;
            anythingToSend = true;

            update->set_variant(comm::ROOM_CLEARED);
            break;
        case SynchronisedEvent::Variant::LEVEL_CHANGED:
            usedPreviousUpdate = true;
            anythingToSend = true;

            update->set_variant(comm::LEVEL_CHANGED);
            break;
        case SynchronisedEvent::Variant::ROOM_DIMENSIONS_CHANGED:
            {
                usedPreviousUpdate = false;

                const auto position = gCoordinator.getComponent<TransformComponent>(updatedEntity).position;
                m_walls.emplace_back(position.x, position.y);
                break;
            }
        case SynchronisedEvent::Variant::ENEMY_GOT_HIT:
            {
                printf("enemy got hit\n");
                // TODO do zrobienia
                break;
            }
        case SynchronisedEvent::Variant::SEND_SPAWNERS_POSITIONS:
            {
                if (!obstacleData.has_value()) continue;
                usedPreviousUpdate = false;

                m_spawners.emplace_back(updatedEntity, sf::Vector2(obstacleData->x, obstacleData->y));
                break;
            }
        default:
            usedPreviousUpdate = false;
            break;
        }
    }

    if (!usedPreviousUpdate) m_updates.mutable_updates()->RemoveLast();

    if (!m_walls.empty())
    {
        auto compressedMapDimensions = sendMapDimensions(m_walls);
        comm::StateUpdate* mapUpdate = m_updates.add_updates();
        mapUpdate->set_variant(comm::StateVariant::MAP_DIMENSIONS_UPDATE);
        mapUpdate->set_compressed_map_dimensions_update(compressedMapDimensions.data(), compressedMapDimensions.size());

        m_walls = {};
        m_isMapDimensionsSent = true;
        anythingToSend = true;
    }

    if (!m_spawners.empty())
    {
        comm::StateUpdate* spawnEnemyRequest = m_updates.add_updates();
        spawnEnemyRequest->set_variant(comm::SPAWN_ENEMY_REQUEST);
        sendSpawnerPosition(*spawnEnemyRequest, m_spawners);

        m_spawners.clear();
        m_areSpawnersSent = true;
        anythingToSend = true;
    }

    if (anythingToSend)
    {
        const auto serialized = m_updates.SerializeAsString();
        m_tcp_socket.send(boost::asio::buffer(addMessageSize(serialized)));
    }
}

void MultiplayerSystem::updateMovement(const std::vector<Entity>& entities)
{
    if (!m_alive) return;

    for (const Entity entity : entities)
    {
        const auto& eventComponent = gCoordinator.getComponent<SynchronisedEvent>(entity);

        switch (eventComponent.variant)
        {
        case SynchronisedEvent::Variant::PLAYER_MOVED:
            {
                const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(m_player_entity);
                const auto& equippedWeapon = gCoordinator.getComponent<EquipmentComponent>(m_player_entity);
                const auto& weaponComponent =
                    gCoordinator.getComponent<WeaponComponent>(equippedWeapon.slots.at(GameType::slotType::WEAPON));

                const auto& windowSize = InputHandler::getInstance()->getWindowSize();
                float scaledPivotX = weaponComponent.pivotPoint.x / static_cast<float>(windowSize.x);
                float scaledPivotY = weaponComponent.pivotPoint.y / static_cast<float>(windowSize.y);

                m_outgoing_movement.set_entity_id(m_player_id);
                m_outgoing_movement.set_position_x(transformComponent.position.x);
                m_outgoing_movement.set_position_y(transformComponent.position.y);
                m_outgoing_movement.set_weapon_pivot_x(scaledPivotX);
                m_outgoing_movement.set_weapon_pivot_y(scaledPivotY);
                m_outgoing_movement.set_direction(transformComponent.scale.x);
                break;
            }
        case SynchronisedEvent::Variant::PLAYER_ATTACKED:
            m_outgoing_movement.set_attack(true);
            break;
        default:
            break;
        }
    }

    // auto tick = sysClock::now();
    // if (!readyToTick(m_last_tick, tick)) return;
    // m_last_tick = tick;

    m_outgoing_movement.set_variant(comm::MovementVariant::PLAYER_MOVEMENT_UPDATE);
    // std::cout << message.ShortDebugString() << "\n";
    auto serialized = m_outgoing_movement.SerializeAsString();
    m_udp_socket.send(boost::asio::buffer(serialized));

    m_outgoing_movement.set_attack(false);
}

void MultiplayerSystem::pollMovement()
{
    size_t received = 0;
    size_t available = m_udp_socket.available();

    while (available > 0)
    {
        received = m_udp_socket.receive(boost::asio::buffer(m_buf));
        available = m_udp_socket.available();
        m_incomming_movement.ParseFromArray(&m_buf, static_cast<int>(received));
        switch (m_incomming_movement.variant())
        {
        case comm::MovementVariant::PLAYER_MOVEMENT_UPDATE:
            {
                uint32_t id = m_incomming_movement.entity_id();
                const auto& r = m_incomming_movement.curr_room();

                if (m_entity_map.contains(id) && m_current_room == glm::ivec2{r.x(), r.y()})
                {
                    Entity& target = m_entity_map[id];
                    auto& transformComponent = gCoordinator.getComponent<TransformComponent>(target);
                    auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(target);

                    const auto& equippedWeapon = gCoordinator.getComponent<EquipmentComponent>(target);
                    const Entity weaponEntity = equippedWeapon.slots.at(GameType::slotType::WEAPON);
                    auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(weaponEntity);
                    auto& weaponRenderComponent = gCoordinator.getComponent<RenderComponent>(weaponEntity);
                    auto& weaponTransformComponent = gCoordinator.getComponent<TransformComponent>(weaponEntity);

                    const auto& windowSize = InputHandler::getInstance()->getWindowSize();

                    float newPivotX = m_incomming_movement.weapon_pivot_x() * static_cast<float>(windowSize.x);
                    float newPivotY = m_incomming_movement.weapon_pivot_y() * static_cast<float>(windowSize.y);
                    // weaponRenderComponent.dirty = true;

                    // TODO: fix remote players' "jumping" - the animation is bugged
                    if (m_incomming_movement.attack())
                    {
                        const Entity fightAction = gCoordinator.createEntity();
                        gCoordinator.addComponent(fightAction, FightActionEvent{target});
                    }

                    float x = m_incomming_movement.position_x();
                    float y = m_incomming_movement.position_y();
                    float r = m_incomming_movement.direction();

                    const float eps = 0.01f;
                    if (!(std::abs(transformComponent.velocity.x - x) > eps ||
                          std::abs(transformComponent.velocity.y - y) > eps || transformComponent.scale.x != r))
                        return;

                    weaponComponent.pivotPoint.x = newPivotX;
                    weaponComponent.pivotPoint.y = newPivotY;

                    transformComponent.velocity.x = x - transformComponent.position.x;
                    transformComponent.velocity.y = y - transformComponent.position.y;
                    transformComponent.scale.x = r;
                    weaponTransformComponent.scale.x = r;

                    colliderComponent.body->SetTransform({convertPixelsToMeters(x), convertPixelsToMeters(y)},
                                                         colliderComponent.body->GetAngle());
                }

                break;
            }
        case comm::MovementVariant::MAP_UPDATE:
            {
                handleMapUpdate(m_incomming_movement.enemy_positions());
                break;
            }
        default:
            break;
        }
    }
}

void MultiplayerSystem::disconnect()
{
    if (!m_connected) return;
    m_tcp_socket.close();
    m_udp_socket.close();
}

std::vector<char> MultiplayerSystem::sendMapDimensions(const std::vector<ObstacleData>& obstacles)
{
    comm::MapDimensionsUpdate mapDimensionsUpdate;
    printf("Obstacle from game size: %lu\n", obstacles.size());

    for (const auto& obstacle : obstacles)
    {
        comm::Obstacle* obstacle_position = mapDimensionsUpdate.add_obstacles();

        obstacle_position->set_left(obstacle.x);
        obstacle_position->set_top(obstacle.y);
    }

    printf("Obstacle ready to be sent to the server: %d\n", mapDimensionsUpdate.obstacles().size());
    std::vector<char> serializedMapDimensions(mapDimensionsUpdate.ByteSizeLong());
    mapDimensionsUpdate.SerializeToArray(serializedMapDimensions.data(), serializedMapDimensions.size());

    uLongf compressedSize = compressBound(serializedMapDimensions.size());
    std::vector<char> compressedMessage(compressedSize);

    if (compress(reinterpret_cast<Bytef*>(compressedMessage.data()), &compressedSize,
                 reinterpret_cast<const Bytef*>(serializedMapDimensions.data()),
                 serializedMapDimensions.size()) == Z_OK)
    {
        compressedMessage.resize(compressedSize);
        return compressedMessage;
    }
    // TODO zrób obsługę błędu kompresji
    printf("Failed to send map dimension update message - compression error\n");
}

void MultiplayerSystem::gatherEnemyAndPlayerPositions()
{
    auto collisionSystem = gCoordinator.getRegisterSystem<CollisionSystem>();
    for (const auto entity : collisionSystem->m_entities)
    {
        if (!gCoordinator.hasComponent<TransformComponent>(entity)) continue;

        auto position = gCoordinator.getComponent<TransformComponent>(entity).position;
        auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
        if (position.x != 0 && position.y != 0)
        {
            if (colliderComponent.tag == "Enemy")
            {
                m_enemyPositions[entity] = {position.x, position.y};
            }
            else if (std::regex_match(colliderComponent.tag, config::playerRegexTag))
            {
                m_playersPositions[entity] = {static_cast<int>(position.x), static_cast<int>(position.y)};
            }
        }
    }
}

void MultiplayerSystem::updateMap()
{
    if (m_enemyPositions.size() < 0 || m_playersPositions.size() < 0) return;

    comm::MapPositionsUpdate mapUpdate;
    for (const auto& enemy : m_enemyPositions)
    {
        if (gCoordinator.getServerEntity(enemy.first) == 0)
        {
            continue;
        }
        comm::Enemy* enemy_position = mapUpdate.add_enemies();
        enemy_position->set_position_x(enemy.second.x);
        enemy_position->set_position_y(enemy.second.y);
        enemy_position->set_id(gCoordinator.getServerEntity(enemy.first));
    }

    for (const auto& player : m_playersPositions)
    {
        comm::Player* player_position = mapUpdate.add_players();
        player_position->set_id(player.first);
        player_position->set_position_x(player.second.x);
        player_position->set_position_y(player.second.y);
    }

    comm::MovementUpdate message;
    message.set_variant(comm::MovementVariant::MAP_UPDATE);
    *message.mutable_map_positions_update() = mapUpdate;

    auto serializedMessage = message.SerializeAsString();

    m_udp_socket.send(boost::asio::buffer(serializedMessage));
}

// TODO w przyszłości będzie też zapytanie o stworznie spawnerów
void MultiplayerSystem::sendSpawnerPosition(comm::StateUpdate& stateUpdate,
                                            const std::vector<std::pair<Entity, sf::Vector2<float>>>& spawners)
{
    std::sort(m_spawners.begin(), m_spawners.end(),
              [](const std::pair<Entity, sf::Vector2<float>>& a, const std::pair<Entity, sf::Vector2<float>>& b)
              { return a.second.x > b.second.x; });

    for (const auto& spawner : spawners)
    {
        comm::Enemy* spawnerPosition = stateUpdate.add_enemy_spawner_positions();
        spawnerPosition->set_position_x(spawner.second.x);
        spawnerPosition->set_position_y(spawner.second.y);
        spawnerPosition->set_id(spawner.first);
    }
    printf("Sending spawn enemy request\n");
}

void MultiplayerSystem::handleMapUpdate(const google::protobuf::RepeatedPtrField<comm::Enemy>& enemyPositions) const
{
    for (const auto& enemy : enemyPositions)
    {
        auto gameEntityId = gCoordinator.getGameEntity(enemy.id());

        if (gameEntityId == 0)
        {
            continue;
        }

        if (gCoordinator.hasComponent<TransformComponent>(gameEntityId))
        {
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(gameEntityId);
            transformComponent.velocity.x = enemy.position_x() * config::m_enemySpeed;
            transformComponent.velocity.y = -enemy.position_y() * config::m_enemySpeed;
        }
    }
}

std::string MultiplayerSystem::addMessageSize(const std::string& serializedMsg)
{
    comm::BytePrefix prefix;
    prefix.set_bytes(serializedMsg.size() + DIFF);

    const auto serialisedPrefix = prefix.SerializeAsString();

    return serialisedPrefix + serializedMsg;
}
