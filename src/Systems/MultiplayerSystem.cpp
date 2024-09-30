#include <chrono>
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
        udp::resolver udp_resolver(m_io_context);
        udp::resolver::results_type udp_endpoints = udp_resolver.resolve(ip, port);

        tcp::resolver tcp_resolver(m_io_context);
        tcp::resolver::results_type tcp_endpoints = tcp_resolver.resolve(ip, port);

        boost::asio::connect(m_udp_socket, udp_endpoints);
        boost::asio::connect(m_tcp_socket, tcp_endpoints);

        m_udp_socket.wait(udp::socket::wait_write);
        m_tcp_socket.wait(tcp::socket::wait_write);
    }
    catch (boost::system::system_error)
    {
        return;
    }

    m_connected = true;
    m_last_tick = sysClock::now();

    auto r1 = new comm::Room();
    m_state.set_allocated_room(r1);

    auto r2 = new comm::Room();
    m_position.set_allocated_curr_room(r2);

    comm::BytePrefix prefixDummy;
    prefixDummy.set_bytes(1);
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

bool MultiplayerSystem::isInsideInitialRoom(const bool change) noexcept
{
    bool ret = m_inside_initial_room;
    if (change) m_inside_initial_room = false;
    return ret;
}

bool MultiplayerSystem::isConnected() const noexcept { return m_connected; }
uint32_t MultiplayerSystem::playerID() const noexcept { return m_player_id; }

comm::GameState MultiplayerSystem::registerPlayer(const Entity player)
{
    std::size_t received = m_tcp_socket.receive(boost::asio::buffer(m_buf));

    comm::GameState gameState;
    gameState.ParseFromArray(&m_buf, static_cast<int>(received));

    uint32_t id = gameState.player_id();
    m_entity_map[id] = player;
    m_player_entity = player;
    m_player_id = id;

    return gameState;
}

comm::StateUpdate MultiplayerSystem::pollStateUpdates()
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
        std::cout << m_state.ShortDebugString() << '\n';

        if (m_state.variant() == comm::ROOM_CHANGED)
        {
            auto r = m_state.room();
            m_current_room.x = r.x();
            m_current_room.y = r.y();
        }
    }
    else
    {
        m_state.set_id(0);
        m_state.set_variant(comm::NONE);
    }
    return m_state;
}

void MultiplayerSystem::entityConnected(const uint32_t id, const Entity entity) noexcept { m_entity_map[id] = entity; }
void MultiplayerSystem::entityDisconnected(const uint32_t id) noexcept { m_entity_map.erase(id); }

void MultiplayerSystem::update()
{
    std::size_t received = 0;
    const std::size_t available = m_udp_socket.available();

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
    }

    auto& transformComponent = gCoordinator.getComponent<TransformComponent>(m_player_entity);

    auto tick = sysClock::now();
    if (!readyToTick(m_last_tick, tick)) return;

    sf::Vector3<float> next = {transformComponent.position.x, transformComponent.position.y,
                               transformComponent.scale.x};
    if (m_last_sent == next) return;

    m_last_tick = tick;
    m_last_sent = next;
    m_position.set_entity_id(m_player_id);
    m_position.set_x(next.x);
    m_position.set_y(next.y);
    m_position.set_direction(next.z);

    auto serialized = m_position.SerializeAsString();
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
