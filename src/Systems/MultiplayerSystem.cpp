#include <cstdint>
#include <iostream>

#include <boost/asio.hpp>
#include <comm.pb.h>

#include "Coordinator.h"
#include "MultiplayerSystem.h"
#include "TransformComponent.h"
#include "Types.h"
#include "boost/system/system_error.hpp"

extern Coordinator gCoordinator;

using boost::asio::ip::tcp;
using boost::asio::ip::udp;
void MultiplayerSystem::setup(const std::string& ip, const std::string& port)
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
    }
    catch (boost::system::system_error)
    {
    }
}

bool MultiplayerSystem::isConnected() const { return m_connected; }
std::uint32_t MultiplayerSystem::playerID() const { return m_player_id; }

std::uint32_t MultiplayerSystem::registerPlayer(Entity player)
{
    std::size_t received = m_tcp_socket.receive(boost::asio::buffer(m_buf));

    m_state.ParseFromArray(&m_buf, received);
    std::uint32_t id = m_state.id();
    m_entity_map[id] = player;
    m_player_entity = player;
    m_player_id = id;

    return id;
}

comm::StateUpdate MultiplayerSystem::pollStateUpdates()
{
    std::size_t available = m_tcp_socket.available();
    comm::StateUpdate state;
    if (available > 0)
    {
        std::size_t received = m_tcp_socket.receive(boost::asio::buffer(m_buf));

        state.ParseFromArray(&m_buf, received);
    }
    else
    {
        state.set_id(0);
        state.set_variant(comm::NONE);
    }
    return state;
}

void MultiplayerSystem::entityConnected(uint32_t id, Entity entity) { m_entity_map[id] = entity; }
void MultiplayerSystem::entityDisconnected(uint32_t id) { m_entity_map.erase(id); }

void MultiplayerSystem::update()
{
    std::size_t received = 0;
    std::size_t available = 0;

    available = m_udp_socket.available();
    // while (available > 0)
    if (available > 0)
    {
        received = m_udp_socket.receive(boost::asio::buffer(m_buf));
        m_position.ParseFromArray(&m_buf, received);
        std::uint32_t id = m_position.entity_id();

        if (m_entity_map.contains(id)) {
            std::cout << m_position.ShortDebugString() << '\n';
            Entity& target = m_entity_map[id];
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(target);

            transformComponent.position = {m_position.x(), m_position.y()};
        }
        // available = m_udp_socket.available();
    }

    auto& transformComponent = gCoordinator.getComponent<TransformComponent>(m_player_entity);

    m_position.set_entity_id(m_player_id);
    m_position.set_x(transformComponent.position.x);
    m_position.set_y(transformComponent.position.y);

    auto serialized = m_position.SerializeAsString();
    m_udp_socket.send(boost::asio::buffer(serialized));
}

void MultiplayerSystem::disconnect()
{
    if (!m_connected) return;
    m_tcp_socket.close();
    m_udp_socket.close();
}
