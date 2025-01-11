#pragma once
#include <optional>

#include <glm/ext/vector_int2.hpp>

#include "GameTypes.h"
#include "Types.h"

struct SynchronisedEvent
{
    enum class UpdateType
    {
        STATE = 1,
        MOVEMENT,
    };

    enum class Variant
    {
        NONE = 0,
        PLAYER_CONNECTED,
        PLAYER_DISCONNECTED,
        PLAYER_KILLED,
        REGISTER_ITEM,
        REQUEST_ITEM_GENERATOR,
        UPDATE_ITEM_ENTITY,
        ITEM_EQUIPPED,
        ROOM_CHANGED,
        ROOM_CLEARED,
        LEVEL_CHANGED,
        PLAYER_MOVED,
        PLAYER_ATTACKED,
        ROOM_DIMENSIONS_CHANGED,
        ENEMY_GOT_HIT,
        SEND_SPAWNERS_POSITIONS,
    };

    UpdateType updateType = UpdateType::STATE;
    Variant variant = Variant::NONE;
    uint32_t entityID = 0;
    Entity entity = 0;
    Entity updatedEntity = 0;
    std::optional<GameType::PickUpInfo> pickUpInfo = std::nullopt;
    std::optional<glm::ivec2> room = std::nullopt;
    std::optional<ObstacleData> obstacleData{};
};
