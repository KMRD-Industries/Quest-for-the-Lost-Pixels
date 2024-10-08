#pragma once
#include <deque>
#include <unordered_map>
#include <vector>

#include <map>
#include "Dungeon.h"
#include "FightSystem.h"
#include "FloorGenerator.h"
#include "PlayerMovementSystem.h"
#include "Room.h"
#include "Timer.h"
#include "Types.h"

#include <TextureSystem.h>

#include "AnimationSystem.h"
#include "CharacterSystem.h"
#include "ChestSystem.h"
#include "CollisionSystem.h"
#include "DoorSystem.h"
#include "EnemySystem.h"
#include "EquipWeaponSystem.h"
#include "HealthBarSystem.h"
#include "InventorySystem.h"
#include "ItemSpawnerSystem.h"
#include "MapSystem.h"
#include "MultiplayerSystem.h"
#include "PassageSystem.h"
#include "RoomListenerSystem.h"
#include "SpawnerSystem.h"
#include "TextTagSystem.h"
#include "TravellingSystem.h"
#include "WeaponsSystem.h"

class Dungeon
{
public:
    Dungeon() : m_entities(MAX_ENTITIES - 1), m_timer(nullptr) {}

    void setupWeaponEntity(Entity player) const;
    void init();
    void addPlayerComponents(Entity player);
    void setupPlayerCollision(Entity player);
    void createRemotePlayer(uint32_t);

    void draw();
    void update(float);

private:
    void setECS();
    void makeSimpleFloor();
    void moveInDungeon(const glm::ivec2& dir);
    void clearDungeon() const;
    void makeStartFloor();
    void moveDownDungeon();
    void loadMap(const std::string& path) const;
    float getSpawnOffset(float, int);
    void changeRoom(const glm::ivec2& dir);
    void updateEnemyPositions(const comm::EnemyPositionsUpdate& enemyPositionsUpdate);

    std::string m_asset_path;
    FloorGenerator m_floorGenerator;
    std::unordered_map<glm::ivec2, Room> m_roomMap;
    glm::ivec2 m_currentPlayerPos;
    std::vector<Entity> m_entities;
    uint32_t m_id{};
    int64_t m_seed{};
    std::set<uint32_t> m_players{};
    std::deque<glm::ivec2> m_moveInDungeon;
    float counter;
    bool m_passedBy;
    std::map<Entity, sf::Vector2<float>> m_enemyPositions;
    std::map<Entity, ObstacleData> m_obstaclePositions;
    std::map<Entity, sf::Vector2<int>> m_playersPositions;
    Timer* m_timer;
    float elapsedTime = 0.0f;
    std::unordered_map<Entity, sf::Vector2<float>> m_enemyPositionsUpdate;

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
    CollisionSystem* m_collisionSystem;
    ChestSystem* m_chestSystem;
    RoomListenerSystem* m_roomListenerSystem;
    ItemSpawnerSystem* m_itemSpawnerSystem;
};
