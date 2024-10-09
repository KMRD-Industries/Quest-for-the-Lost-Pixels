#include "Room.h"

#include <RenderSystem.h>
#include <string>

#include "AnimationSystem.h"
#include "DoorComponent.h"
#include "EnemyComponent.h"
#include "FloorComponent.h"
#include "GameUtility.h"
#include "PassageComponent.h"
#include "Paths.h"

void Room::init() {}

void Room::draw() const {}

void Room::update() {}

int Room::getFloorID() const { return m_floorID; }


std::string Room::getMap() const
{
    const std::string& path = "/maps/floor_0" + std::to_string(m_floorID) + "/map_";
    const std::string suffix{".json"};
    return std::string(ASSET_PATH) + path + m_mapID + suffix;
}
