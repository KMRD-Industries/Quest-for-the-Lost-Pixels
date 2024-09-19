#include <cstdint>
#include <iostream>

#include <boost/asio.hpp>
#include <comm.pb.h>
#include <vector>

#include "Coordinator.h"
#include "MultiplayerSystem.h"
#include "TransformComponent.h"
#include "Types.h"
#include "boost/system/system_error.hpp"

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
    }
    catch (boost::system::system_error)
    {
    }
}

bool MultiplayerSystem::isConnected() const noexcept { return m_connected; }
std::uint32_t MultiplayerSystem::playerID() const noexcept { return m_player_id; }

std::uint32_t MultiplayerSystem::registerPlayer(const Entity player)
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
    const std::size_t available = m_tcp_socket.available();
    comm::StateUpdate state;
    if (available > 0)
    {
        // temporary solution - msg length should always be 4
        // reading all available bytes will cause ignoring of all but 1 messages
        std::vector<char> buf(4);
        std::size_t received = m_tcp_socket.read_some(boost::asio::buffer(buf));

        state.ParseFromArray(buf.data(), received);
        std::cout << state.ShortDebugString() << '\n';
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

    if (available > 0)
    {
        received = m_udp_socket.receive(boost::asio::buffer(m_buf));
        m_position.ParseFromArray(&m_buf, received);
        std::uint32_t id = m_position.entity_id();

        if (m_entity_map.contains(id))
        {
            Entity& target = m_entity_map[id];
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(target);

            transformComponent.position = {m_position.x(), m_position.y()};
        }
    }

    auto& transformComponent = gCoordinator.getComponent<TransformComponent>(m_player_entity);

    if (m_last_sent == transformComponent.position) return;

    m_last_sent = transformComponent.position;
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
