#pragma once
#include <unordered_map>

#include <SFML/System/Vector2.hpp>
#include <boost/asio.hpp>
#include <comm.pb.h>
#include <glm/vec2.hpp>

#include "System.h"
#include "Types.h"
#include "glm/ext/vector_int2.hpp"

using boost::asio::ip::tcp;
using boost::asio::ip::udp;
class MultiplayerSystem : public System
{
private:
    bool m_connected = false;
    Entity m_player_entity = 0;
    uint32_t m_player_id = 0;
    boost::asio::io_context m_io_context;
    tcp::socket m_tcp_socket;
    udp::socket m_udp_socket;
    std::array<char, 4096> m_buf{};
    sf::Vector2<float> m_last_sent{};
    glm::ivec2 m_current_room{};
    comm::PositionUpdate m_position{};
    comm::PositionUpdate m_incomming_pos{};
    comm::StateUpdate m_state{};
    std::unordered_map<uint32_t, Entity> m_entity_map{};
    std::unordered_map<Entity, glm::ivec2> m_entity_rooms{};

public:
    MultiplayerSystem() noexcept : m_io_context(), m_udp_socket(m_io_context), m_tcp_socket(m_io_context) {};
    void setup(const std::string& ip, const std::string& port) noexcept;
    void entityConnected(const uint32_t id, const Entity entity) noexcept;
    void entityDisconnected(const uint32_t id) noexcept;
    void roomChanged(const int x, const int y) noexcept;
    void update();
    void disconnect();

    uint32_t registerPlayer(const Entity player);
    uint32_t playerID() const noexcept;
    glm::ivec2 getCurrentRoom() const noexcept;
    std::optional<glm::ivec2> getEntityRoom(const Entity entity) const noexcept;
    comm::StateUpdate pollStateUpdates();
    bool isConnected() const noexcept;
};
