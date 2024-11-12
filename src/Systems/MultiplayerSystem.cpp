#include <cstdint>
#include <iostream>

#include <boost/asio.hpp>
#include <comm.pb.h>
#include <vector>

#include "ColliderComponent.h"
#include "Config.h"
#include "Coordinator.h"
#include "Helpers.h"
#include "MultiplayerSystem.h"

#include <zconf.h>
#include <zlib.h>

#include "CharacterComponent.h"
#include "MultiplayerComponent.h"
#include "SFML/System/Vector3.hpp"
#include "TransformComponent.h"
#include "Types.h"

#include "boost/system/system_error.hpp"
#include "box2d/b2_body.h"
#include "glm/ext/vector_int2.hpp"

extern Coordinator gCoordinator;

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

void MultiplayerSystem::init(){};

void MultiplayerSystem::setup(const std::string& ip, const std::string& port) noexcept
{
    try
    {
        udp::resolver udp_resolver(m_io_context);
        udp::resolver::results_type udp_endpoints = udp_resolver.resolve(ip, port);

        tcp::resolver tcp_resolver(m_io_context);
        tcp::resolver::results_type tcp_endpoints = tcp_resolver.resolve(ip, port);

        boost::asio::connect(m_udp_socket, udp_endpoints);
        boost::asio::connect(m_tcp_socket, tcp_endpoints);

        m_udp_socket.wait(udp::socket::wait_write);
        m_tcp_socket.wait(tcp::socket::wait_write);
        m_connected = true;

        auto r1 = new comm::Room();
        m_state.set_allocated_room(r1);

        auto r2 = new comm::Room();
        m_position.set_allocated_curr_room(r2);
    }
    catch (boost::system::system_error)
    {
    }
}

void MultiplayerSystem::setRoom(const glm::ivec2& room) noexcept
{
    m_current_room = room;

    auto r1 = m_state.release_room();
    r1->set_x(room.x);
    r1->set_y(room.y);
    m_state.set_allocated_room(r1);

    auto r2 = m_position.release_curr_room();
    r2->set_x(room.x);
    r2->set_y(room.y);
    m_position.set_allocated_curr_room(r2);
}

glm::ivec2& MultiplayerSystem::getRoom() noexcept { return m_current_room; }

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

bool MultiplayerSystem::isConnected() const noexcept { return m_connected; }
std::uint32_t MultiplayerSystem::playerID() const noexcept { return m_player_id; }

comm::GameState MultiplayerSystem::registerPlayer(const Entity player)
{
    std::size_t received = m_tcp_socket.receive(boost::asio::buffer(m_buf));

    comm::GameState gameState;
    gameState.ParseFromArray(&m_buf, int(received));

    std::uint32_t id = gameState.player_id();
    m_entity_map[id] = player;
    m_player_entity = player;
    m_player_id = id;

    return gameState;
}

comm::StateUpdate MultiplayerSystem::pollStateUpdates()
{
    const std::size_t available = m_tcp_socket.available();
    comm::StateUpdate state;
    if (available > 0)
    {
        // TODO: actual fix (works most of the time at the moment)
        // std::vector<char> buf(4);
        // std::size_t received = m_tcp_socket.read_some(boost::asio::buffer(buf));

        std::size_t received = m_tcp_socket.receive(boost::asio::buffer(m_buf));

        state.ParseFromArray(m_buf.data(), int(received));
        // std::cout << state.ShortDebugString() << '\n';

        if (state.variant() == comm::ROOM_CHANGED)
        {
            auto r = state.room();
            m_current_room.x = r.x();
            m_current_room.y = r.y();
        }
    }
    else
    {
        state.set_id(0);
        state.set_variant(comm::NONE);
    }
    return state;
}

void MultiplayerSystem::entityConnected(const uint32_t id, const Entity entity) noexcept { m_entity_map[id] = entity; }
void MultiplayerSystem::entityDisconnected(const uint32_t id) noexcept { m_entity_map.erase(id); }

void MultiplayerSystem::update(const float deltaTime)
{
    std::size_t received = 0;
    const std::size_t available = m_udp_socket.available();

    // std::cout << "[update] Bytes available: " << available << std::endl;
    if (available > 0)
    {
        received = m_udp_socket.receive(boost::asio::buffer(m_buf));
        comm::StateUpdate m_message;
        m_message.ParseFromArray(m_buf.data(), int(received));
        switch (m_message.variant())
        {
        case comm::PLAYER_POSITION_UPDATE:
            {
                handlePlayerPositionUpdate(m_message.positionupdate());
                break;
            }
        case comm::MAP_UPDATE:
            {
                handleMapUpdate(m_message.enemypositionsupdate());
                break;
            }
        default:
            break;
        }
    }

    auto& transformComponent = gCoordinator.getComponent<TransformComponent>(m_player_entity);

    sf::Vector3<float> next = {transformComponent.position.x, transformComponent.position.y,
                               transformComponent.scale.x};
    if (m_last_sent == next) return;

    m_last_sent = next;
    m_position.set_entity_id(m_player_id);
    m_position.set_x(next.x);
    m_position.set_y(next.y);
    m_position.set_direction(next.z);
    comm::StateUpdate update;
    // TODO jak zadziała to do zmiany
    *update.mutable_positionupdate() = m_position;
    update.set_variant(comm::PLAYER_POSITION_UPDATE);
    auto serialized = update.SerializeAsString();

    // auto serialized = m_position.SerializeAsString();
    m_udp_socket.send(boost::asio::buffer(serialized));

    std::deque<Entity> entities;
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
        entities.push_back(entity);
    }

    for (const auto entity : entities)
    {
        gCoordinator.removeComponent<MultiplayerComponent>(entity);
    }

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

    if (m_frameTime += deltaTime; m_frameTime >= configSingleton.GetConfig().oneFrameTime * 2000 && areSpawnersSent)
    {
        gatherEnemyAndPlayerPositions();
        updateMap(m_enemyPositions, m_playersPositions);
        m_frameTime -= configSingleton.GetConfig().oneFrameTime * 2000;
    }
}

std::string MultiplayerSystem::addMessageSize(const std::string& serializedMsg)
{
    std::string msgWithSize;
    msgWithSize.reserve(2 + serializedMsg.size());

    uint16_t size = htons(static_cast<uint16_t>(serializedMsg.size()));
    msgWithSize.append(reinterpret_cast<const char*>(&size), 2);

    msgWithSize.append(serializedMsg);
    return msgWithSize;
}

void MultiplayerSystem::receiveMessages() {}

void MultiplayerSystem::sendPlayerPosition()
{
    auto& transformComponent = gCoordinator.getComponent<TransformComponent>(m_player_entity);

    sf::Vector3<float> next = {transformComponent.position.x, transformComponent.position.y,
                               transformComponent.scale.x};
    if (m_last_sent == next) return;

    m_last_sent = next;
    m_position.set_entity_id(m_player_id);
    m_position.set_x(next.x);
    m_position.set_y(next.y);
    m_position.set_direction(next.z);
    comm::StateUpdate update;
    // TODO jak zadziała to do zmiany
    *update.mutable_positionupdate() = m_position;
    update.set_variant(comm::PLAYER_POSITION_UPDATE);
    auto serialized = update.SerializeAsString();

    // auto serialized = m_position.SerializeAsString();
    m_udp_socket.send(boost::asio::buffer(serialized));
}

void MultiplayerSystem::disconnect()
{
    if (!m_connected) return;
    delete m_state.release_room();
    delete m_position.release_curr_room();
    m_tcp_socket.close();
    m_udp_socket.close();
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
    printf("Map dimensions size: %d\n", mapDimensionsUpdate.obstacles_size());

    std::vector<char> serializedMapDimensions(mapDimensionsUpdate.ByteSizeLong());
    mapDimensionsUpdate.SerializeToArray(serializedMapDimensions.data(), serializedMapDimensions.size());

    std::cout << "Original size: " << serializedMapDimensions.size() << "\n";
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
        std::cout << "Sending compressed map dimension update message, compressed size: " << compressedMessage.size()
                  << "\n";

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
        enemy_position->set_x(enemy.second.x);
        enemy_position->set_y(enemy.second.y);
        enemy_position->set_id(gCoordinator.getServerEntity(enemy.first));
    }

    for (const auto& player : players)
    {
        comm::Player* player_position = mapUpdate.add_players();
        player_position->set_id(player.first);
        player_position->set_x(player.second.x);
        player_position->set_y(player.second.y);
    }

    comm::StateUpdate message;
    message.set_variant(comm::StateVariant::MAP_UPDATE);
    *message.mutable_mappositionsupdate() = mapUpdate;

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
        comm::Enemy* spawnerPosition = spawnEnemyRequest.add_enemypositions();
        spawnerPosition->set_x(spawner.second.x);
        spawnerPosition->set_y(spawner.second.y);
        spawnerPosition->set_id(spawner.first);
    }

    comm::StateUpdate message;
    *message.mutable_enemypositionsupdate() = spawnEnemyRequest;
    message.set_variant(comm::StateVariant::SPAWN_ENEMY_REQUEST);

    auto serializedMessage = message.SerializeAsString();
    printf("sending spawners position\n");
    for (int i = 0; i < 5; ++i)
    {
        m_tcp_socket.send(boost::asio::buffer(addMessageSize(serializedMessage)));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void MultiplayerSystem::handlePlayerPositionUpdate(const comm::PositionUpdate& positionUpdate)
{
    m_position = positionUpdate;
    std::uint32_t id = m_position.entity_id();
    auto r = m_position.curr_room();

    if (m_entity_map.contains(id) && m_current_room == glm::ivec2{r.x(), r.y()})
    {
        Entity& target = m_entity_map[id];
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(target);
        auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(target);

        float x = m_position.x();
        float y = m_position.y();
        float r = m_position.direction();

        transformComponent.position.x = x;
        transformComponent.position.y = y;
        transformComponent.scale.x = r;
        colliderComponent.body->SetTransform({convertPixelsToMeters(x), convertPixelsToMeters(y)},
                                             colliderComponent.body->GetAngle());
    }
}

void MultiplayerSystem::handleMapUpdate(const comm::EnemyPositionsUpdate& enemyPositionsUpdate)
{
    for (auto enemy : enemyPositionsUpdate.enemypositions())
    {
        auto gameEntityId = gCoordinator.getGameEntity(enemy.id());

        if (gameEntityId == 0)
        {
            continue;
        }

        if (gCoordinator.hasComponent<TransformComponent>(gameEntityId))
        {
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(gameEntityId);

            // TODO przsyłam pozycje serwera i odejmuję aktualną pozycję na kliencie
            transformComponent.velocity.x = enemy.x() * 50;
            transformComponent.velocity.y = -enemy.y() * 50;
        }
    }
};

void MultiplayerSystem::enemyGotHitUpdate(Entity enemyId)
{

}
