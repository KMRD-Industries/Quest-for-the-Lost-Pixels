#pragma once

#include <boost/asio.hpp>
#include <comm.pb.h>
#include <unordered_map>

#include "System.h"
#include "Types.h"

using boost::asio::ip::tcp;
using boost::asio::ip::udp;
class MultiplayerSystem : public System
{
private:
    bool m_connected = false;
    Entity m_player_entity = 0;
    std::uint32_t m_player_id = 0;
    boost::asio::io_context m_io_context;
    tcp::socket m_tcp_socket;
    udp::socket m_udp_socket;
    std::array<char, 4096> m_buf{};
    comm::PositionUpdate m_position;
    comm::StateUpdate m_state;
    std::unordered_map<std::uint32_t, Entity> m_entity_map;

public:
    MultiplayerSystem() : m_io_context(), m_udp_socket(m_io_context), m_tcp_socket(m_io_context){};
    void setup(const std::string& ip, const std::string& port);
    void entityConnected(std::uint32_t id, Entity entity);
    void entityDisconnected(std::uint32_t id);
    void update();
    void disconnect();

    std::uint32_t registerPlayer(Entity player);
    std::uint32_t playerID() const;
    comm::StateUpdate pollStateUpdates();
    bool isConnected() const;
};
