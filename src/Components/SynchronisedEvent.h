#pragma once
#include <optional>

#include <glm/ext/vector_int2.hpp>

#include "GameTypes.h"
#include "Types.h"

struct SynchronisedEvent
{
    enum Variant
    {
        NONE = 0,
        PLAYER_CONNECTED = 1,
        PLAYER_DISCONNECTED = 2,
        PLAYER_KILLED = 3,
        REGISTER_ITEM = 4,
        REQUEST_ITEM_GENERATOR = 5,
        UPDATE_ITEM_ENTITY = 6,
        ITEM_EQUIPPED = 7,
        ROOM_CHANGED = 8,
        ROOM_CLEARED = 9,
        LEVEL_CHANGED = 10,
        ATTACK = 11,
    };

    Variant variant = NONE;
    uint32_t entityID = 0;
    Entity entity = 0;
    Entity updatedEntity = 0;
    std::optional<GameType::PickUpInfo> pickUpInfo = std::nullopt;
    std::optional<glm::ivec2> room = std::nullopt;
};
