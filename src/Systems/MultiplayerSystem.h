#pragma once
#include <unordered_map>
#include <SFML/System/Vector3.hpp>
#include <boost/asio.hpp>
#include <comm.pb.h>

#include "GameTypes.h"
#include "System.h"
#include "Timer.h"
#include "Types.h"
#include "glm/ext/vector_int2.hpp"
#include <zlib.h>

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
    sf::Vector3<float> m_last_sent{};
    glm::ivec2 m_current_room{};
    comm::PositionUpdate m_position{};
    comm::StateUpdate m_state{};
    std::unordered_map<std::uint32_t, Entity> m_entity_map{};
    std::unordered_map<std::uint32_t, ObstacleData> m_walls{};

public:
    MultiplayerSystem() noexcept : m_io_context(), m_udp_socket(m_io_context), m_tcp_socket(m_io_context){};
    void setup(const std::string& ip, const std::string& port) noexcept;
    void setRoom(const glm::ivec2& room) noexcept;
    void entityConnected(const std::uint32_t id, const Entity entity) noexcept;
    void entityDisconnected(const std::uint32_t id) noexcept;
    void roomChanged(const glm::ivec2& room);
    void init();
    void update();
    void updateMap(const std::map<Entity, sf::Vector2<float>>& enemies,
                   const std::map<Entity, sf::Vector2<int>>& players);
    void sendMapDimensions(const std::unordered_map<Entity, ObstacleData>& obstacles);
    void sendSpawnerPosition(std::vector<std::pair<Entity, sf::Vector2<float>>> spawners);
    void updateEnemyHp(Entity id, float hp, sf::Vector2<float> position);
    void disconnect();

    bool isConnected() const noexcept;
    std::uint32_t playerID() const noexcept;
    glm::ivec2& getRoom() noexcept;
    comm::GameState registerPlayer(const Entity player);
    comm::StateUpdate pollStateUpdates();
};
