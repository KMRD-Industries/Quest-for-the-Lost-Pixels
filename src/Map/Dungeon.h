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
#include "DealDMGToEnemySystem.h"
#include "DoorSystem.h"
#include "EnemySystem.h"
#include "HealthBarSystem.h"
#include "InventorySystem.h"
#include "ItemSpawnerSystem.h"
#include "ItemSystem.h"
#include "MapSystem.h"
#include "MultiplayerSystem.h"
#include "PassageSystem.h"
#include "Paths.h"
#include "RoomListenerSystem.h"
#include "SpawnerSystem.h"
#include "State.h"
#include "TextTagSystem.h"
#include "TravellingSystem.h"
#include "WeaponBindSystem.h"
#include "WeaponsSystem.h"

class Dungeon
{
public:
    Dungeon() : m_entities(MAX_ENTITIES - 1){};

    void init();
    void render(sf::RenderTexture& window);
    void addPlayerComponents(Entity player);
    void moveDownDungeon();
    void setupPlayerCollision(Entity player);
    void setupWeaponEntity(Entity player);
    void setupHelmetEntity(Entity player) const;
    void update(float deltaTime);
    void makeStartFloor();

    StateChangeCallback m_stateChangeCallback;

private:
    void setECS();
    void makeSimpleFloor();
    void createRemotePlayer(uint32_t id);
    void moveInDungeon(const glm::ivec2& dir);
    void changeRoom(const glm::ivec2& dir);
    void clearDungeon();
    void loadMap(const std::string& path) const;
    float getSpawnOffset(float position, int id);
    void checkForEndOfTheGame();
    void updateEnemyPositions(const comm::EnemyPositionsUpdate& enemyPositionsUpdate);
    void sendRoomDimensions();
    void updateMap();
    void mapServerIdToGameId(const comm::SpawningEnemiesResponse& ids) const;

    std::string m_asset_path{ASSET_PATH};
    FloorGenerator m_floorGenerator{};
    std::unordered_map<glm::ivec2, Room> m_roomMap{};
    glm::ivec2 m_currentPlayerPos{};
    std::vector<Entity> m_entities{};
    uint32_t m_id{};
    int64_t m_seed{};
    std::set<uint32_t> m_players{};
    std::deque<glm::ivec2> m_moveInDungeon;
    float counter;
    bool m_passedBy;
    int m_dungeonDepth{1};
    bool m_endGame{};
    Entity m_weapon{};
    std::map<Entity, sf::Vector2<float>> m_enemyPositions;
    std::unordered_map<Entity, ObstacleData> m_obstaclePositions;
    std::map<Entity, sf::Vector2<int>> m_playersPositions;
    Timer* m_timer;
    float elapsedTime = 0.0f;
    std::unordered_map<Entity, sf::Vector2<float>> m_enemyPositionsUpdate;
    std::queue<comm::Enemy> m_enemyQueue;

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
    InventorySystem* m_inventorySystem;
    CollisionSystem* m_collisionSystem;
    ChestSystem* m_chestSystem;
    RoomListenerSystem* m_roomListenerSystem;
    ItemSpawnerSystem* m_itemSpawnerSystem;
    ItemSystem* m_itemSystem;
    WeaponBindSystem* m_weaponBindSystem;
    DealDMGToEnemySystem* m_dealDMGSystem;
};
