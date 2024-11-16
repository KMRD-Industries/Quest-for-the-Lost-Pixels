#pragma once
#include <optional>
#include <unordered_map>

#include <SFML/System/Vector3.hpp>
#include <boost/asio.hpp>
#include <comm.pb.h>
#include <glm/ext/vector_int2.hpp>

#include "System.h"
#include "Types.h"

struct ItemGenerator
{
    uint32_t id;
    uint32_t gen;
    comm::ItemType type;
};

struct MultiplayerDungeonUpdate
{
    enum Variant
    {
        NONE = 0,
        REGISTER_PLAYER = 1,
        CHANGE_ROOM = 2,
        CHANGE_LEVEL = 3,
    };

    Variant variant = NONE;
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
    comm::StateUpdateSeries m_updates{};

    std::vector<MultiplayerDungeonUpdate> m_dungeon_updates{};

    void pollMovement();
    void pollState();
    void updateState(const std::vector<Entity>& entities);
    void updateMovement(const std::vector<Entity>& entities);

public:
    MultiplayerSystem() noexcept : m_io_context(), m_udp_socket(m_io_context), m_tcp_socket(m_io_context) {};
    void setup(const std::string_view& ip, const std::string_view& port) noexcept;
    void setPlayer(const uint32_t id, const Entity entity);
    void setRoom(const glm::ivec2& room) noexcept;
    void update();
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
};
