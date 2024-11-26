#include <chrono>
#include <cstdint>
#include <iostream>

#include <boost/asio.hpp>
#include <comm.pb.h>
#include <vector>

#include "ColliderComponent.h"
#include "Config.h"
#include "Coordinator.h"
#include "EquipmentComponent.h"
#include "FightActionEvent.h"
#include "GameTypes.h"
#include "Helpers.h"
#include "InputHandler.h"
#include "MultiplayerSystem.h"

#include <zconf.h>
#include <zlib.h>

#include "CharacterComponent.h"
#include "MultiplayerComponent.h"
#include "SFML/System/Vector3.hpp"
#include "TransformComponent.h"
#include "Types.h"

#include "WeaponComponent.h"
#include "boost/system/system_error.hpp"

extern Coordinator gCoordinator;

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

    m_state.set_allocated_room(new comm::Room());
    m_outgoing_movement.set_allocated_curr_room(new comm::Room());
    m_incomming_movement.set_allocated_curr_room(new comm::Room());

    comm::BytePrefix prefixDummy;
    prefixDummy.set_bytes(300);
    m_prefix_size = static_cast<int>(prefixDummy.ByteSizeLong());
    m_prefix_buf.resize(m_prefix_size);
}

void MultiplayerSystem::setRoom(const glm::ivec2& room) noexcept
{
    m_current_room = room;

    auto r1 = m_state.release_room();
    r1->set_x(room.x);
    r1->set_y(room.y);
    m_state.set_allocated_room(r1);

    auto r2 = m_outgoing_movement.release_curr_room();
    r2->set_x(room.x);
    r2->set_y(room.y);
    m_outgoing_movement.set_allocated_curr_room(r2);
}

const glm::ivec2& MultiplayerSystem::getRoom() const noexcept { return m_current_room; }

const ItemGenerator& MultiplayerSystem::getItemGenerator()
{
    m_generator_ready = false;

    m_state.set_variant(comm::REQUEST_ITEM_GENERATOR);
    m_state.mutable_player()->set_id(m_player_id);

    const auto serialized = m_state.SerializeAsString();

    m_tcp_socket.send(boost::asio::buffer(serialized));
    return m_item_generator;
}

void MultiplayerSystem::roomChanged(const glm::ivec2& room)
{
    setRoom(room);

    m_state.set_variant(comm::ROOM_CHANGED);
    auto serialized = m_state.SerializeAsString();
    printf("Sending room change update\n");
    isMapDimensionsSent = false;
    areSpawnersSent = false;

    m_tcp_socket.send(boost::asio::buffer(addMessageSize(serialized)));
}

void MultiplayerSystem::roomCleared()
{
    m_state.set_variant(comm::ROOM_CLEARED);
    const auto serialized = m_state.SerializeAsString();

    m_tcp_socket.send(boost::asio::buffer(serialized));
}

void MultiplayerSystem::onAttack() { m_outgoing_movement.set_attack(true); }

bool MultiplayerSystem::isInsideInitialRoom(const bool change) noexcept
{
    bool ret = m_inside_initial_room;
    if (change) m_inside_initial_room = false;
    return ret;
}

bool MultiplayerSystem::isConnected() const noexcept { return m_connected; }
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

    const auto& nextItem = initialInfo.next_item();
    m_item_generator.id = nextItem.id();
    m_item_generator.gen = nextItem.gen();
    m_item_generator.type = nextItem.type();
    m_generator_ready = true;

    return initialInfo;
}

const comm::StateUpdate& MultiplayerSystem::pollStateUpdates()
{
    size_t available = m_tcp_socket.available();
    if (available >= m_prefix_size)
    {
        // read message byte size
        size_t received = m_tcp_socket.read_some(boost::asio::buffer(m_prefix_buf, m_prefix_size));
        m_prefix.ParseFromArray(m_prefix_buf.data(), m_prefix_size);

        uint32_t msg_size = m_prefix.bytes();

        available = m_tcp_socket.available();
        while (available < msg_size) available = m_tcp_socket.available();

        // read exactly msg_size bytes
        received = m_tcp_socket.read_some(boost::asio::buffer(m_buf, msg_size));

        m_state.ParseFromArray(m_buf.data(), static_cast<int>(received));
        // std::cout << m_state.ShortDebugString() << '\n';

        if (msg_size > 0)
        {
            printf("<<< Msg is still alive with size: %d and state: %d\n", msg_size, m_state.variant());
        }
        switch (m_state.variant())
        {
        case comm::ROOM_CHANGED:
            {
                auto r = m_state.room();
                setRoom({r.x(), r.y()});
                break;
            }
        case comm::REQUEST_ITEM_GENERATOR:
            {
                const auto& nextItem = m_state.item();
                m_generator_ready = true;
                m_item_generator.id = nextItem.id();
                m_item_generator.gen = nextItem.gen();
                m_item_generator.type = nextItem.type();
            }
        default:
        }
    }
    else
    {
        m_state.set_variant(comm::NONE);
    }
    return m_state;
}

void MultiplayerSystem::playerConnected(const uint32_t id, const Entity entity) noexcept { m_entity_map[id] = entity; }
void MultiplayerSystem::playerDisconnected(const uint32_t id) noexcept
{
    gCoordinator.getComponent<ColliderComponent>(m_entity_map[id]).toDestroy = true;
    for (auto& slot : gCoordinator.getComponent<EquipmentComponent>(m_entity_map[id]).slots)
    {
        if (slot.second != 0)
        {
            m_registered_items.erase(slot.second);
            gCoordinator.getComponent<ColliderComponent>(slot.second).toDestroy = true;
        }
    }
    m_entity_map.erase(id);
}

void MultiplayerSystem::registerItem(const uint32_t id, const Entity entity) { m_registered_items[id] = entity; }

void MultiplayerSystem::updateItemEntity(const Entity oldEntity, const Entity newEntity)
{
    for (const auto& p : m_registered_items)
    {
        if (p.second != oldEntity) continue;

        m_registered_items[p.first] = newEntity;
        return;
    }
}

Entity MultiplayerSystem::getItemEntity(const uint32_t id)
{
    if (m_registered_items.contains(id)) return m_registered_items[id];
    return 0;
}
void MultiplayerSystem::itemEquipped(const GameType::PickUpInfo& pickUpInfo)
{
    for (const auto& p : m_registered_items)
    {
        if (p.second != pickUpInfo.itemEntity) continue;

        m_state.set_variant(comm::ITEM_EQUIPPED);
        m_state.mutable_player()->set_id(m_player_id);
        auto item = m_state.mutable_item();
        item->set_id(p.first);

        switch (pickUpInfo.slot)
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

        const auto serialized = m_state.SerializeAsString();

        m_tcp_socket.send(boost::asio::buffer(serialized));
        return;
    }
}

void MultiplayerSystem::update(const float deltaTime)
{
    std::size_t received = 0;
    const std::size_t available = m_udp_socket.available();

    if (available > 0)
    {
        received = m_udp_socket.receive(boost::asio::buffer(m_buf));
        comm::StateUpdate m_message;
        m_message.ParseFromArray(&m_buf, int(received));
        switch (m_message.variant())
        {
        case comm::PLAYER_POSITION_UPDATE:
            {
                handlePlayerPositionUpdate(m_message);
                break;
            }
        case comm::MAP_UPDATE:
            {
                handleMapUpdate(m_message.enemy_positions_update());
                break;
            }
        default:
            break;
        }
    }

    sendPlayerPosition();

    // std::deque<Entity> m_multiplayerEntities;
    for (auto entity : m_entities)
    {
        auto multiplayerComponent = gCoordinator.getComponent<MultiplayerComponent>(entity);
        switch (multiplayerComponent.type)
        {
        case multiplayerType::ROOM_DIMENSIONS_CHANGED:
            {
                if (!gCoordinator.hasComponent<TransformComponent>(entity)) continue;

                const auto position = gCoordinator.getComponent<TransformComponent>(entity).position;
                auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
                if (colliderComponent.tag == "Wall")
                {
                    ObstacleData obstacle{position.x, position.y};
                    m_walls.insert({entity, obstacle});
                }
                break;
            }
        case multiplayerType::ENEMY_GOT_HIT:
            {
                printf("enemy got hit\n");
                // TODO do zrobienia
                break;
            }
        case multiplayerType::SEND_SPAWNERS_POSITIONS:
            {
                m_spawners.push_back(
                    std::make_pair(entity, gCoordinator.getComponent<TransformComponent>(entity).position));
                break;
            }
        default:
            break;
        }
        m_multiplayerEntities.push_back(entity);
    }

    for (const auto entity : m_multiplayerEntities)
    {
        gCoordinator.removeComponent<MultiplayerComponent>(entity);
    }
    m_multiplayerEntities.clear();

    if (!m_walls.empty())
    {
        sendMapDimensions(m_walls);
        m_walls.clear();
        isMapDimensionsSent = true;
    }

    if (!m_spawners.empty())
    {
        sendSpawnerPosition(m_spawners);
        m_spawners.clear();
        areSpawnersSent = true;
    }

    if (m_frameTime += deltaTime; m_frameTime >= configSingleton.GetConfig().oneFrameTime * frames && areSpawnersSent)
    {
        gatherEnemyAndPlayerPositions();
        updateMap(m_enemyPositions, m_playersPositions);
        m_frameTime -= configSingleton.GetConfig().oneFrameTime * frames;
    }
}

void MultiplayerSystem::disconnect()
{
    if (!m_connected) return;
    delete m_state.release_room();
    delete m_incomming_movement.release_curr_room();
    delete m_outgoing_movement.release_curr_room();
    m_tcp_socket.close();
    m_udp_socket.close();
}

void MultiplayerSystem::handlePlayerPositionUpdate(const comm::StateUpdate& m_message)
{
    m_incomming_movement = m_message.movement_update();
    // m_incomming_movement.ParseFromArray(&m_buf, int(received));
    uint32_t id = m_incomming_movement.entity_id();
    auto r = m_incomming_movement.curr_room();

    if (m_entity_map.contains(id) && m_current_room == glm::ivec2{r.x(), r.y()})
    {
        Entity& target = m_entity_map[id];
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(target);
        auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(target);

        const auto& equippedWeapon = gCoordinator.getComponent<EquipmentComponent>(target);
        const Entity& weaponEntity = equippedWeapon.slots.at(GameType::slotType::WEAPON);
        auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(weaponEntity);
        auto& weaponTransformComponent = gCoordinator.getComponent<TransformComponent>(weaponEntity);

        const auto& windowSize = InputHandler::getInstance()->getWindowSize();

        weaponComponent.pivotPoint.x = m_incomming_movement.weapon_pivot_x() * static_cast<float>(windowSize.x);
        weaponComponent.pivotPoint.y = m_incomming_movement.weapon_pivot_y() * static_cast<float>(windowSize.y);

        if (m_incomming_movement.attack())
        {
            const Entity fightAction = gCoordinator.createEntity();
            gCoordinator.addComponent(fightAction, FightActionEvent{target});
        }

        float x = m_incomming_movement.position_x();
        float y = m_incomming_movement.position_y();
        float r = m_incomming_movement.direction();

        transformComponent.velocity.x = x - transformComponent.position.x;
        transformComponent.velocity.y = y - transformComponent.position.y;
        transformComponent.scale.x = r;
        weaponTransformComponent.scale.x = r;

        colliderComponent.body->SetTransform({convertPixelsToMeters(x), convertPixelsToMeters(y)},
                                             colliderComponent.body->GetAngle());
    }
}

void MultiplayerSystem::sendPlayerPosition()
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

    comm::StateUpdate update{};
    update.set_variant(comm::StateVariant::PLAYER_POSITION_UPDATE);
    // update.set_allocated_movement_update(&m_outgoing_movement);
    *update.mutable_movement_update() = m_outgoing_movement;

    auto serialized = update.SerializeAsString();
    m_udp_socket.send(boost::asio::buffer(serialized));

    m_outgoing_movement.set_attack(false);
}

void MultiplayerSystem::sendMapDimensions(const std::unordered_map<Entity, ObstacleData>& obstacles)
{
    comm::MapDimensionsUpdate mapDimensionsUpdate;

    for (const auto& obstacle : obstacles)
    {
        comm::Obstacle* obstacle_position = mapDimensionsUpdate.add_obstacles();

        obstacle_position->set_left(obstacle.second.left);
        obstacle_position->set_top(obstacle.second.top);
    }

    std::vector<char> serializedMapDimensions(mapDimensionsUpdate.ByteSizeLong());
    mapDimensionsUpdate.SerializeToArray(serializedMapDimensions.data(), serializedMapDimensions.size());

    uLongf compressedSize = compressBound(serializedMapDimensions.size());
    std::vector<char> compressedMessage(compressedSize);

    if (compress(reinterpret_cast<Bytef*>(compressedMessage.data()), &compressedSize,
                 reinterpret_cast<const Bytef*>(serializedMapDimensions.data()),
                 serializedMapDimensions.size()) == Z_OK)
    {
        compressedMessage.resize(compressedSize);

        comm::StateUpdate message;
        message.set_variant(comm::StateVariant::MAP_DIMENSIONS_UPDATE);

        message.set_compressed_map_dimensions_update(compressedMessage.data(), compressedMessage.size());
        auto serializedMessage = message.SerializeAsString();

        m_tcp_socket.send(boost::asio::buffer(addMessageSize(serializedMessage)));
    }
    else
    {
        // TODO zrób obsługę błędu kompresji
        printf("Failed to send map dimension update message - compression error\n");
    }
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

void MultiplayerSystem::updateMap(const std::map<Entity, sf::Vector2<float>>& enemies,
                                  const std::map<Entity, sf::Vector2<int>>& players)
{
    comm::MapPositionsUpdate mapUpdate;
    for (const auto& enemy : enemies)
    {
        if (gCoordinator.getServerEntity(enemy.first) == 0)
        {
            // TODO imo powinno być return
            continue;
        }
        comm::Enemy* enemy_position = mapUpdate.add_enemies();
        enemy_position->set_position_x(enemy.second.x);
        enemy_position->set_position_y(enemy.second.y);
        enemy_position->set_id(gCoordinator.getServerEntity(enemy.first));
    }

    for (const auto& player : players)
    {
        comm::Player* player_position = mapUpdate.add_players();
        player_position->set_id(player.first);
        player_position->set_position_x(player.second.x);
        player_position->set_position_y(player.second.y);
    }

    comm::StateUpdate message;
    message.set_variant(comm::StateVariant::MAP_UPDATE);
    *message.mutable_map_positions_update() = mapUpdate;

    auto serializedMessage = message.SerializeAsString();

    m_udp_socket.send(boost::asio::buffer(serializedMessage));
}

// TODO w przyszłości będzie też zapytanie o stworznie spawnerów
void MultiplayerSystem::sendSpawnerPosition(std::vector<std::pair<Entity, sf::Vector2<float>>> spawners)
{
    std::sort(m_spawners.begin(), m_spawners.end(),
              [](const std::pair<Entity, sf::Vector2<float>>& a, const std::pair<Entity, sf::Vector2<float>>& b)
              { return a.second.x > b.second.x; });

    comm::EnemyPositionsUpdate spawnEnemyRequest;

    for (const auto& spawner : spawners)
    {
        comm::Enemy* spawnerPosition = spawnEnemyRequest.add_enemy_positions();
        spawnerPosition->set_position_x(spawner.second.x);
        spawnerPosition->set_position_y(spawner.second.y);
        spawnerPosition->set_id(spawner.first);
    }

    comm::StateUpdate message;
    *message.mutable_enemy_positions_update() = spawnEnemyRequest;
    message.set_variant(comm::StateVariant::SPAWN_ENEMY_REQUEST);

    auto serializedMessage = message.SerializeAsString();
    printf("sending spawners position\n");
    for (int i = 0; i < 5; ++i)
    {
        m_tcp_socket.send(boost::asio::buffer(addMessageSize(serializedMessage)));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void MultiplayerSystem::handleMapUpdate(const comm::EnemyPositionsUpdate& enemyPositionsUpdate)
{
    for (auto enemy : enemyPositionsUpdate.enemy_positions())
    {
        auto gameEntityId = gCoordinator.getGameEntity(enemy.id());

        if (gameEntityId == 0)
        {
            continue;
        }

        if (gCoordinator.hasComponent<TransformComponent>(gameEntityId))
        {
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(gameEntityId);

            transformComponent.velocity.x = enemy.position_x() * enemy_speed;
            transformComponent.velocity.y = -enemy.position_y() * enemy_speed;
        }
    }
};

std::string MultiplayerSystem::addMessageSize(const std::string& serializedMsg)
{
    std::string msgWithSize;
    msgWithSize.reserve(2 + serializedMsg.size());

    uint16_t size = htons(static_cast<uint16_t>(serializedMsg.size()));
    msgWithSize.append(reinterpret_cast<const char*>(&size), 2);

    msgWithSize.append(serializedMsg);
    return msgWithSize;
}
