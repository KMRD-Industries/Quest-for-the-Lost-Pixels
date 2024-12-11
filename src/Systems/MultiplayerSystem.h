#pragma once
#include <optional>
#include <unordered_map>
#include <boost/asio.hpp>
#include <comm.pb.h>
#include <glm/ext/vector_int2.hpp>

#include "GameTypes.h"
#include "System.h"
#include "Types.h"

struct ItemGenerator
{
    uint32_t id;
    uint32_t gen;
    comm::ItemType type;
};

#include "CollisionSystem.h"

struct MultiplayerDungeonUpdate
{
    enum class Variant
    {
        NONE = 0,
        REGISTER_PLAYER,
        CHANGE_ROOM,
        CHANGE_LEVEL,
    };

    Variant variant = Variant::NONE;
    std::optional<glm::ivec2> room = std::nullopt;
    comm::Player player{};
};

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

class MultiplayerSystem : public System
{
private:
    bool m_connected = false;
    bool m_alive = true;
    bool m_inside_initial_room = true;
    Entity m_player_entity = 0;
    uint32_t m_player_id = 0;
    int64_t m_seed = 0;
    float m_frameTime = 0.0;


    boost::asio::io_context m_io_context;
    tcp::socket m_tcp_socket;
    udp::socket m_udp_socket;
    std::array<char, 4096> m_buf{};

    std::chrono::system_clock::time_point m_last_tick{};
    glm::ivec2 m_current_room{};
    std::unordered_map<uint32_t, Entity> m_entity_map{};

    bool m_generator_ready = false;
    ItemGenerator m_item_generator{};
    std::unordered_map<uint32_t, Entity> m_registered_items{};

    int m_prefix_size{};
    std::vector<char> m_prefix_buf{};
    comm::BytePrefix m_prefix{};
    comm::MovementUpdate m_incomming_movement{};
    comm::MovementUpdate m_outgoing_movement{};
    comm::StateUpdate m_state{};
    std::vector<ObstacleData> m_walls{};
    std::map<Entity, sf::Vector2<float>> m_enemyPositions;
    std::map<Entity, sf::Vector2<int>> m_playersPositions;
    std::vector<std::pair<Entity, sf::Vector2<float>>> m_spawners{};
    CollisionSystem* m_collisionSystem;
    bool m_isMapDimensionsSent{};
    bool m_areSpawnersSent{};
    std::deque<Entity> m_multiplayerEntities;
    comm::StateUpdateSeries m_updates{};

    std::vector<MultiplayerDungeonUpdate> m_dungeon_updates{};

    void pollMovement();
    void pollState();
    std::vector<char> sendMapDimensions(const std::vector<ObstacleData>& obstacles);
    void updateState(const std::vector<Entity>& entities);
    void updateMovement(const std::vector<Entity>& entities);

public:
    MultiplayerSystem() noexcept : m_io_context(), m_udp_socket(m_io_context), m_tcp_socket(m_io_context) {};
    void setup(const std::string_view& ip, const std::string_view& port) noexcept;
    void setPlayer(const uint32_t id, const Entity entity);
    void setRoom(const glm::ivec2& room) noexcept;
    void clearRemoteDungeonUpdates();
    void update(const float deltaTime);
    void updateMap(const std::map<Entity, sf::Vector2<float>>& enemies,
                   const std::map<Entity, sf::Vector2<int>>& players);
    void sendMapDimensions(const std::unordered_map<Entity, ObstacleData>& obstacles);
    void gatherEnemyAndPlayerPositions();
    void sendSpawnerPosition(comm::StateUpdate& stateUpdate,
                             const std::vector<std::pair<Entity, sf::Vector2<float>>>& spawners);
    void handleMapUpdate(const google::protobuf::RepeatedPtrField<comm::Enemy>& enemyPositions) const;
    void disconnect();

    bool isConnected() const noexcept;
    bool isInsideInitialRoom(const bool change) noexcept;
    uint32_t playerID() const noexcept;
    const glm::ivec2& getRoom() const noexcept;
    const ItemGenerator& getItemGenerator();
    Entity getItemEntity(const uint32_t id);
    int64_t getSeed();
    const std::unordered_map<uint32_t, Entity>& getPlayers();
    const std::vector<MultiplayerDungeonUpdate>& getRemoteDungeonUpdates();
    comm::InitialInfo registerPlayer(const Entity player);
    std::string addMessageSize(const std::string& serializedMsg);

    static constexpr int m_frames = 2000;
    static constexpr int m_enemySpeed = 50;
};
