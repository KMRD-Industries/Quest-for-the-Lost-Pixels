#pragma once
#include <unordered_map>

#include <SFML/System/Vector3.hpp>
#include <boost/asio.hpp>
#include <comm.pb.h>
#include <glm/ext/vector_int2.hpp>

#include "GameTypes.h"

#include <zlib.h>
#include "GameTypes.h"
#include "System.h"
#include "Timer.h"
#include "Types.h"

struct ItemGenerator
{
    uint32_t id;
    uint32_t gen;
    comm::ItemType type;
};

#include "CollisionSystem.h"

using boost::asio::ip::tcp;
using boost::asio::ip::udp;
class MultiplayerSystem : public System
{
private:
    bool m_connected = false;
    bool m_inside_initial_room = true;
    Entity m_player_entity = 0;
    uint32_t m_player_id = 0;

    boost::asio::io_context m_io_context;
    tcp::socket m_tcp_socket;
    udp::socket m_udp_socket;
    std::array<char, 4096> m_buf{};

    std::chrono::system_clock::time_point m_last_tick{};
    glm::ivec2 m_current_room{};

    bool m_generator_ready = false;
    ItemGenerator m_item_generator{};
    std::unordered_map<uint32_t, Entity> m_registered_items{};

    int m_prefix_size{};
    std::vector<char> m_prefix_buf{};
    comm::BytePrefix m_prefix{};
    comm::MovementUpdate m_incomming_movement{};
    comm::MovementUpdate m_outgoing_movement{};
    comm::StateUpdate m_state{};
    std::unordered_map<std::uint32_t, Entity> m_entity_map{};
    std::unordered_map<std::uint32_t, ObstacleData> m_walls{};
    std::map<Entity, sf::Vector2<float>> m_enemyPositions;
    std::map<Entity, sf::Vector2<int>> m_playersPositions;
    std::vector<std::pair<Entity, sf::Vector2<float>>> m_spawners{};
    float m_frameTime{};
    CollisionSystem* m_collisionSystem;
    void enemyGotHitUpdate(Entity enemyId);
    bool isMapDimensionsSent{};
    bool areSpawnersSent{};
    std::deque<Entity> m_multiplayerEntities;

public:
    MultiplayerSystem() noexcept : m_io_context(), m_udp_socket(m_io_context), m_tcp_socket(m_io_context) {};
    void setup(const std::string_view& ip, const std::string_view& port) noexcept;
    void setRoom(const glm::ivec2& room) noexcept;
    void playerConnected(const uint32_t id, const Entity entity) noexcept;
    void playerDisconnected(const uint32_t id) noexcept;
    void registerItem(const uint32_t id, const Entity entity);
    void updateItemEntity(const Entity oldEntity, const Entity newEntity);
    void itemEquipped(const GameType::PickUpInfo& entity);
    void roomChanged(const glm::ivec2& room);
    void roomCleared();
    void onAttack();
    void update();
    void update(const float deltaTime);
    void updateMap(const std::map<Entity, sf::Vector2<float>>& enemies,
                   const std::map<Entity, sf::Vector2<int>>& players);
    void sendMapDimensions(const std::unordered_map<Entity, ObstacleData>& obstacles);
    void gatherEnemyAndPlayerPositions();
    void sendSpawnerPosition(std::vector<std::pair<Entity, sf::Vector2<float>>> spawners);
    void handlePlayerPositionUpdate(const comm::MovementUpdate& positionUpdate);
    void handleMapUpdate(const comm::EnemyPositionsUpdate& enemyPositionUpdate);
    void disconnect();

    bool isConnected() const noexcept;
    bool isInsideInitialRoom(const bool change) noexcept;
    uint32_t playerID() const noexcept;
    const glm::ivec2& getRoom() const noexcept;
    const ItemGenerator& getItemGenerator();
    Entity getItemEntity(const uint32_t id);
    comm::InitialInfo registerPlayer(const Entity player);
    const comm::StateUpdate& pollStateUpdates();
    std::string addMessageSize(const std::string& serializedMsg);
    void receiveMessages();
    void sendPlayerPosition();
};
