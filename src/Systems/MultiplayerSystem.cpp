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
        //TODO wyślij tutaj info o wymiarach pierwszej mapy i potem spradź jak działa wysłanie mapy razem z updatem pokoju
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

    m_tcp_socket.send(boost::asio::buffer(serialized));
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

void MultiplayerSystem::update()
{
    std::size_t received = 0;
    const std::size_t available = m_udp_socket.available();

    //    std::cout << "[update] Bytes available: " << available << std::endl;
    if (available > 0)
    {
        received = m_udp_socket.receive(boost::asio::buffer(m_buf));
        m_position.ParseFromArray(&m_buf, int(received));
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

        // comm::StateUpdate stateUpdate;
        // stateUpdate.ParseFromArray(&m_buf, received);
        // //        m_position.ParseFromArray(&m_buf, received);
        // std::cout << "Received message...";
        // switch (stateUpdate.variant())
        // {
        // case comm::StateVariant::MAP_UPDATE:
        //     std::cout << "Map update...\n";
        //     break;
        // case comm::StateVariant::PLAYER_POSITION_UPDATE:
        //     //idk czy git
        //     m_position = stateUpdate.positionupdate();
        //     const std::uint32_t id = m_position.entity_id();
        //     std::cout << "Position update from: " << "id:" << id <<"\n";

        //     if (m_entity_map.contains(id))
        //     {
        //         Entity& target = m_entity_map[id];
        //         auto& transformComponent = gCoordinator.getComponent<TransformComponent>(target);

        //         transformComponent.position = {m_position.x(), m_position.y()};
        //     }
        //     break;
        // default:
        //     std::cout << "Unknown type!\n";
        //     break;
        // }
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

    auto serialized = m_position.SerializeAsString();
    m_udp_socket.send(boost::asio::buffer(serialized));

    // std::cout << "Player: " << m_player_id << " is moving to: " << m_position.x() << ", " << m_position.y() << "\n";
    // comm::StateUpdate update;
    // update.set_variant(comm::PLAYER_POSITION_UPDATE);
    //*(update.mutable_positionupdate()) = m_position;
    // auto serialized = update.SerializeAsString();
}

void MultiplayerSystem::disconnect()
{
    if (!m_connected) return;
    delete m_state.release_room();
    delete m_position.release_curr_room();
    m_tcp_socket.close();
    m_udp_socket.close();
}

void MultiplayerSystem::updateMap(const std::map<Entity, sf::Vector2<float>>& enemies,
                                  const std::map<Entity, ObstacleData>& obstacles,
                                  const std::map<Entity, sf::Vector2<int>>& players)
{
    comm::MapPositionsUpdate mapUpdate;
    // std::cout << "another update!!!!\n";
    for (const auto& enemy : enemies)
    {
        if (gCoordinator.getServerEntity(enemy.first) == 0)
        {
            //TODO imo powinno być return
            continue;
        }
        // std::cout << "enemy's position: x: " << enemy.second.x << " y: " << enemy.second.y << "\n";
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
    // std::cout << "Sending map updated message, size: " << serializedMessage.size() << "\n";

    m_tcp_socket.send(boost::asio::buffer(serializedMessage));
}

void MultiplayerSystem::setMapDimensions(const std::map<Entity, ObstacleData>& obstacles){
    comm::MapDimensionsUpdate mapDimensionsUpdate;
    for (const auto& obstacle : obstacles)
    {
        comm::Obstacle* obstacle_position = mapDimensionsUpdate.add_obstacles();

        obstacle_position->set_height(obstacle.second.height);
        obstacle_position->set_width(obstacle.second.width);
        obstacle_position->set_left(obstacle.second.left);
        obstacle_position->set_top(obstacle.second.top);
    }
    comm::StateUpdate message;
    message.set_variant(comm::StateVariant::MAP_DIMENSIONS_UPDATE);
    *message.mutable_mapdimensionsupdate() = mapDimensionsUpdate;

    auto serializedMessage = message.SerializeAsString();
    // std::cout << "Sending map dimension update message, size: " << serializedMessage.size() << "\n";

    m_tcp_socket.send(boost::asio::buffer(serializedMessage));
}

void MultiplayerSystem::askForEnemyId(const Entity enemyId, const sf::Vector2<float> position)
{
    const auto spawnRequest = new comm::Enemy;
    spawnRequest->set_id(enemyId);
    spawnRequest->set_x(position.x);
    spawnRequest->set_y(position.y);

    comm::StateUpdate message;
    message.set_variant(comm::StateVariant::SPAWN_ENEMY_REQUEST);
    *message.mutable_spawnenemyrequest() = *spawnRequest;

    auto serializedMessage = message.SerializeAsString();
    m_tcp_socket.send(boost::asio::buffer(serializedMessage));
}

