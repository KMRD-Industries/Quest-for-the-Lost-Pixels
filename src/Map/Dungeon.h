#pragma once
#include <deque>
#include <unordered_map>
#include <vector>

#include "Dungeon.h"
#include "FightSystem.h"
#include "FloorGenerator.h"
#include "PlayerMovementSystem.h"
#include "Room.h"
#include "Types.h"

#include <TextureSystem.h>

#include "AnimationSystem.h"
#include "CharacterSystem.h"
#include "DoorSystem.h"
#include "EnemySystem.h"
#include "EquipWeaponSystem.h"
#include "HealthBarSystem.h"
#include "InventorySystem.h"
#include "MapSystem.h"
#include "MultiplayerSystem.h"
#include "PassageSystem.h"
#include "SpawnerSystem.h"
#include "TextTagSystem.h"
#include "TravellingSystem.h"
#include "WeaponsSystem.h"

class Dungeon
{
public:
    Dungeon() : m_entities(MAX_ENTITIES - 1){};

    void setupWeaponEntity(Entity player);
    void init();
    void addPlayerComponents(Entity player);
    void setupPlayerCollision(Entity player);
    void draw();
    void update();

private:
    void setECS();
    void makeSimpleFloor();
    void moveInDungeon(const glm::ivec2& dir);
    void clearDungeon();
    void makeStartFloor();
    void moveDownDungeon();

    std::string m_asset_path;
    FloorGenerator m_floorGenerator;
    std::unordered_map<glm::ivec2, Room> m_roomMap;
    glm::ivec2 m_currentPlayerPos;
    std::vector<Entity> m_entities;
    std::uint32_t m_id;
    std::deque<glm::ivec2> m_moveInDungeon;
    float counter;
    bool m_passedBy;

    PlayerMovementSystem* m_playerMovementSystem;
    MultiplayerSystem* m_multiplayerSystem;
    CharacterSystem* m_characterSystem;
    MapSystem* m_mapSystem;
    DoorSystem* m_doorSystem;
    PassageSystem* m_passageSystem;
    TravellingSystem* m_travellingSystem;
    TextureSystem* m_textureSystem;
    AnimationSystem* m_animationSystem;
    EnemySystem* m_enemySystem;
    SpawnerSystem* m_spawnerSystem;
    WeaponSystem* m_weaponSystem;
    TextTagSystem* m_textTagSystem;
    HealthBarSystem* m_healthBarSystem;
    EquipWeaponSystem* m_equipWeaponSystem;
    InventorySystem* m_inventorySystem;
};
