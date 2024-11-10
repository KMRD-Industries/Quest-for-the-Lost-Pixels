#include <chrono>
#include <cstdint>
#include <iostream>

#include <boost/asio.hpp>
#include <comm.pb.h>
#include <vector>

#include "ColliderComponent.h"
#include "Config.h"
#include "Coordinator.h"
#include "EquipmentComponent.h"
#include "FightActionEvent.h"
#include "GameTypes.h"
#include "Helpers.h"
#include "InputHandler.h"
#include "MultiplayerSystem.h"
#include "SynchronisedEvent.h"
#include "TransformComponent.h"
#include "Types.h"

#include "WeaponComponent.h"

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
        boost::asio::steady_timer timer(m_io_context);

        timer.expires_after(std::chrono::seconds(5));
        udp::resolver udp_resolver(m_io_context);
        udp::resolver::results_type udp_endpoints = udp_resolver.resolve(ip, port);

        tcp::resolver tcp_resolver(m_io_context);
        tcp::resolver::results_type tcp_endpoints = tcp_resolver.resolve(ip, port);

        m_udp_socket.connect(*udp_endpoints.begin());
        m_tcp_socket.async_connect(*tcp_endpoints.begin(),
                                   [&](const boost::system::error_code& ec)
                                   {
                                       timer.cancel();
                                       if (!ec) m_connected = true;
                                   });

        timer.async_wait(
            [&](const boost::system::error_code& ec)
            {
                if (!m_connected && !ec) m_tcp_socket.cancel();
            });

        m_io_context.run();
    }
    catch (boost::system::system_error)
    {
        return;
    }

    if (!m_connected) return;

    m_last_tick = sysClock::now();

    m_state.set_allocated_room(new comm::Room());
    m_outgoing_movement.set_allocated_curr_room(new comm::Room());
    m_incomming_movement.set_allocated_curr_room(new comm::Room());

    comm::BytePrefix prefixDummy;
    prefixDummy.set_bytes(1);
    m_prefix_size = static_cast<int>(prefixDummy.ByteSizeLong());
    m_prefix_buf.resize(m_prefix_size);

    m_updates.mutable_updates()->Reserve(64);
}

void MultiplayerSystem::setRoom(const glm::ivec2& room) noexcept
{
    m_current_room = room;

    auto r1 = m_state.release_room();
    r1->set_x(room.x);
    r1->set_y(room.y);
    m_state.set_allocated_room(r1);

    auto r2 = m_outgoing_movement.release_curr_room();
    r2->set_x(room.x);
    r2->set_y(room.y);
    m_outgoing_movement.set_allocated_curr_room(r2);
}

const glm::ivec2& MultiplayerSystem::getRoom() const noexcept { return m_current_room; }

const ItemGenerator& MultiplayerSystem::getItemGenerator() { return m_item_generator; }

void MultiplayerSystem::onAttack() { m_outgoing_movement.set_attack(true); }

bool MultiplayerSystem::isInsideInitialRoom(const bool change) noexcept
{
    bool ret = m_inside_initial_room;
    if (change) m_inside_initial_room = false;
    return ret;
}

bool MultiplayerSystem::isConnected() const noexcept { return m_connected; }
void MultiplayerSystem::setPlayer(const uint32_t id, const Entity entity) { m_entity_map[id] = entity; }
uint32_t MultiplayerSystem::playerID() const noexcept { return m_player_id; }

comm::InitialInfo MultiplayerSystem::registerPlayer(const Entity playerEntity)
{
    size_t received = m_tcp_socket.receive(boost::asio::buffer(m_buf));

    comm::InitialInfo initialInfo;
    initialInfo.ParseFromArray(&m_buf, static_cast<int>(received));

    uint32_t id = initialInfo.player().id();
    m_entity_map[id] = playerEntity;
    m_player_entity = playerEntity;
    m_player_id = id;

    m_seed = initialInfo.seed();

    const auto& nextItem = initialInfo.next_item();
    m_item_generator.id = nextItem.id();
    m_item_generator.gen = nextItem.gen();
    m_item_generator.type = nextItem.type();
    m_generator_ready = true;

    return initialInfo;
}

const comm::StateUpdate& MultiplayerSystem::pollStateUpdates()
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

        switch (m_state.variant())
        {
        case comm::ROOM_CHANGED:
            {
                auto r = m_state.room();
                setRoom({r.x(), r.y()});
                break;
            }
        case comm::REQUEST_ITEM_GENERATOR:
            {
                const auto& nextItem = m_state.item();
                m_generator_ready = true;
                m_item_generator.id = nextItem.id();
                m_item_generator.gen = nextItem.gen();
                m_item_generator.type = nextItem.type();
            }
        default:
        }
    }
    else
    {
        m_state.set_variant(comm::NONE);
    }
    return m_state;
}

Entity MultiplayerSystem::getItemEntity(const uint32_t id)
{
    if (m_registered_items.contains(id)) return m_registered_items[id];
    return 0;
}

int64_t MultiplayerSystem::getSeed() { return m_seed; }

const std::unordered_map<uint32_t, Entity>& MultiplayerSystem::getPlayers() { return m_entity_map; }

void MultiplayerSystem::update()
{
    if (!m_connected)
    {
        std::deque<Entity> eventEntities;

        for (const Entity entity : m_entities) eventEntities.push_back(entity);

        while (!eventEntities.empty())
        {
            gCoordinator.destroyEntity(eventEntities.front());
            eventEntities.pop_front();
        }
        return;
    }

    comm::StateUpdate* update = nullptr;
    bool anythingToSend = false, usedPreviousUpdate = true;

    m_updates.mutable_updates()->Clear();

    for (const Entity eventEntity : m_entities)
    {
        const auto& [variant, entityID, entity, updatedEntity, pickUpInfo, room] =
            gCoordinator.getComponent<SynchronisedEvent>(eventEntity);

        if (usedPreviousUpdate) update = m_updates.add_updates();

        switch (variant)
        {
        case SynchronisedEvent::PLAYER_CONNECTED:
            usedPreviousUpdate = false;
            if (entity == 0 || entityID == 0) continue;

            m_entity_map[entityID] = entity;
            break;

        case SynchronisedEvent::PLAYER_DISCONNECTED:
            usedPreviousUpdate = false;
            if (entityID == 0 || !m_entity_map.contains(entityID)) continue;

            gCoordinator.getComponent<ColliderComponent>(m_entity_map[entityID]).toDestroy = true;
            for (auto& slot : gCoordinator.getComponent<EquipmentComponent>(m_entity_map[entityID]).slots)
            {
                if (slot.second != 0)
                {
                    m_registered_items.erase(slot.second);
                    gCoordinator.getComponent<ColliderComponent>(slot.second).toDestroy = true;
                }
            }
            m_entity_map.erase(entityID);
            break;

        case SynchronisedEvent::PLAYER_KILLED:
            usedPreviousUpdate = true;
            anythingToSend = true;

            for (auto& slot : gCoordinator.getComponent<EquipmentComponent>(m_entity_map[m_player_id]).slots)
            {
                if (slot.second != 0)
                {
                    m_registered_items.erase(slot.second);
                    gCoordinator.getComponent<ColliderComponent>(slot.second).toDestroy = true;
                }
            }

            update->set_variant(comm::PLAYER_DIED);
            update->mutable_player()->set_id(m_player_id);
            break;

        case SynchronisedEvent::REGISTER_ITEM:
            usedPreviousUpdate = false;
            if (entity == 0 || entityID == 0) continue;

            m_registered_items[entityID] = entity;
            break;

        case SynchronisedEvent::REQUEST_ITEM_GENERATOR:
            usedPreviousUpdate = true;
            anythingToSend = true;

            update->set_variant(comm::REQUEST_ITEM_GENERATOR);
            update->mutable_player()->set_id(m_player_id);
            break;

        case SynchronisedEvent::UPDATE_ITEM_ENTITY:
            usedPreviousUpdate = false;
            if (entity == 0 || updatedEntity == 0) continue;

            for (const auto& p : m_registered_items)
            {
                if (p.second != entity) continue;

                m_registered_items[p.first] = updatedEntity;
                break;
            }
            break;

        case SynchronisedEvent::ITEM_EQUIPPED:
            usedPreviousUpdate = true;
            anythingToSend = true;

            if (!pickUpInfo.has_value())
            {
                usedPreviousUpdate = false;
                continue;
            }

            for (const auto& p : m_registered_items)
            {
                if (p.second != pickUpInfo->itemEntity) continue;

                update->set_variant(comm::ITEM_EQUIPPED);
                update->mutable_player()->set_id(m_player_id);

                auto item = update->mutable_item();
                item->set_id(p.first);

                switch (pickUpInfo->slot)
                {
                case GameType::slotType::WEAPON:
                    item->set_type(comm::WEAPON);
                    break;
                case GameType::slotType::HELMET:
                    item->set_type(comm::HELMET);
                    break;
                case GameType::slotType::BODY_ARMOUR:
                    item->set_type(comm::ARMOUR);
                    break;
                default:
                    break;
                }
                break;
            }
            break;

        case SynchronisedEvent::ROOM_CHANGED:
            usedPreviousUpdate = true;
            anythingToSend = true;

            if (!room.has_value())
            {
                usedPreviousUpdate = false;
                continue;
            }

            setRoom(*room);

            update->set_variant(comm::ROOM_CHANGED);
            update->mutable_room()->set_x(room->x);
            update->mutable_room()->set_y(room->y);
            break;

        case SynchronisedEvent::ROOM_CLEARED:
            usedPreviousUpdate = true;
            anythingToSend = true;

            update->set_variant(comm::ROOM_CLEARED);
            break;

        case SynchronisedEvent::LEVEL_CHANGED:
            usedPreviousUpdate = true;
            anythingToSend = true;

            update->set_variant(comm::LEVEL_CHANGED);
            break;

        default:
            usedPreviousUpdate = false;
            break;
        }
    }

    std::deque<Entity> eventEntities;

    for (const Entity entity : m_entities) eventEntities.push_back(entity);

    while (!eventEntities.empty())
    {
        gCoordinator.destroyEntity(eventEntities.front());
        eventEntities.pop_front();
    }

    if (!usedPreviousUpdate) m_updates.mutable_updates()->RemoveLast();

    if (anythingToSend)
    {
        const auto serialized = m_updates.SerializeAsString();
        m_tcp_socket.send(boost::asio::buffer(serialized));
    }
}

void MultiplayerSystem::updateMovement()
{
    size_t received = 0;
    const size_t available = m_udp_socket.available();

    if (available > 0)
    {
        received = m_udp_socket.receive(boost::asio::buffer(m_buf));
        m_incomming_movement.ParseFromArray(&m_buf, int(received));
        uint32_t id = m_incomming_movement.entity_id();
        auto r = m_incomming_movement.curr_room();

        if (m_entity_map.contains(id) && m_current_room == glm::ivec2{r.x(), r.y()})
        {
            Entity& target = m_entity_map[id];
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(target);
            auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(target);

            const auto& equippedWeapon = gCoordinator.getComponent<EquipmentComponent>(target);
            const Entity& weaponEntity = equippedWeapon.slots.at(GameType::slotType::WEAPON);
            auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(weaponEntity);
            auto& weaponTransformComponent = gCoordinator.getComponent<TransformComponent>(weaponEntity);

            const auto& windowSize = InputHandler::getInstance()->getWindowSize();

            weaponComponent.pivotPoint.x = m_incomming_movement.weapon_pivot_x() * static_cast<float>(windowSize.x);
            weaponComponent.pivotPoint.y = m_incomming_movement.weapon_pivot_y() * static_cast<float>(windowSize.y);

            if (m_incomming_movement.attack())
            {
                const Entity fightAction = gCoordinator.createEntity();
                gCoordinator.addComponent(fightAction, FightActionEvent{target});
            }

            float x = m_incomming_movement.position_x();
            float y = m_incomming_movement.position_y();
            float r = m_incomming_movement.direction();

            transformComponent.velocity.x = x - transformComponent.position.x;
            transformComponent.velocity.y = y - transformComponent.position.y;
            transformComponent.scale.x = r;
            weaponTransformComponent.scale.x = r;

            colliderComponent.body->SetTransform({convertPixelsToMeters(x), convertPixelsToMeters(y)},
                                                 colliderComponent.body->GetAngle());
        }
    }

    if (!m_alive) return;

    auto tick = sysClock::now();
    if (!readyToTick(m_last_tick, tick)) return;
    m_last_tick = tick;

    const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(m_player_entity);
    const auto& equippedWeapon = gCoordinator.getComponent<EquipmentComponent>(m_player_entity);
    const auto& weaponComponent =
        gCoordinator.getComponent<WeaponComponent>(equippedWeapon.slots.at(GameType::slotType::WEAPON));

    const auto& windowSize = InputHandler::getInstance()->getWindowSize();
    float scaledPivotX = weaponComponent.pivotPoint.x / static_cast<float>(windowSize.x);
    float scaledPivotY = weaponComponent.pivotPoint.y / static_cast<float>(windowSize.y);

    m_outgoing_movement.set_entity_id(m_player_id);
    m_outgoing_movement.set_position_x(transformComponent.position.x);
    m_outgoing_movement.set_position_y(transformComponent.position.y);
    m_outgoing_movement.set_weapon_pivot_x(scaledPivotX);
    m_outgoing_movement.set_weapon_pivot_y(scaledPivotY);
    m_outgoing_movement.set_direction(transformComponent.scale.x);

    auto serialized = m_outgoing_movement.SerializeAsString();
    m_udp_socket.send(boost::asio::buffer(serialized));

    m_outgoing_movement.set_attack(false);
}

void MultiplayerSystem::disconnect()
{
    if (!m_connected) return;
    delete m_state.release_room();
    delete m_incomming_movement.release_curr_room();
    delete m_outgoing_movement.release_curr_room();
    m_tcp_socket.close();
    m_udp_socket.close();
}
