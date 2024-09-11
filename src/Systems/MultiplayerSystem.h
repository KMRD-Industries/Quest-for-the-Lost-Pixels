#pragma once
#include <unordered_map>

#include <SFML/System/Vector2.hpp>
#include <boost/asio.hpp>
#include <comm.pb.h>

#include "System.h"
#include "Types.h"

using boost::asio::ip::tcp;
using boost::asio::ip::udp;
class MultiplayerSystem : public System
{
private:
    bool m_connected = false;
    Entity m_player_entity = 0;
    std::uint32_t m_player_id = 1;
    boost::asio::io_context m_io_context;
    tcp::socket m_tcp_socket;
    udp::socket m_udp_socket;
    std::array<char, 4096> m_buf{};
    sf::Vector2<float> m_last_sent{};
    comm::PositionUpdate m_position{};
    comm::StateUpdate m_state{};
    std::unordered_map<std::uint32_t, Entity> m_entity_map{};

public:
    MultiplayerSystem() noexcept : m_io_context(), m_udp_socket(m_io_context), m_tcp_socket(m_io_context){};
    void setup(const std::string& ip, const std::string& port) noexcept;
    void entityConnected(const std::uint32_t id, const Entity entity) noexcept;
    void entityDisconnected(const std::uint32_t id) noexcept;
    void update();
    void disconnect();

    std::uint32_t registerPlayer(const Entity player);
    std::uint32_t playerID() const noexcept;
    comm::StateUpdate pollStateUpdates();
    bool isConnected() const noexcept;
};
