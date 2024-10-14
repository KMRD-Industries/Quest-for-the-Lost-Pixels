#pragma once
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "Config.h"
#include "PublicConfig.h"

using json = nlohmann::json;

class PublicConfigSingleton
{
public:
    PublicConfigSingleton()
    {
    }

    void LoadConfig(const std::string& filename)
    {
        std::ifstream configFile(filename);
        if (configFile.is_open())
        {
            json j;
            configFile >> j;
            parseJsonConfig(j);
        }
        else
        {
            std::cerr << "Can't load config using default values." << std::endl;
            useDefaultConfig();
        }
    }

    const PublicConfig& GetConfig() const
    {
        return publicConfig;
    }

private:
    PublicConfig publicConfig;

    void useDefaultConfig()
    {
        // @formatter:off — disable formatter after this line

        // Engine settings
        publicConfig.debugMode = config::debugMode; // Enables or disables debug mode
        publicConfig.gameScale = config::gameScale; // Scale of the game (affects all visuals
        publicConfig.meterToPixelRatio = config::meterToPixelRatio; // Conversion ratio for Box2D physics
        publicConfig.pixelToMeterRatio = config::pixelToMeterRatio; // Conversion ratio for game to physics
        publicConfig.tileHeight = config::tileHeight; // Height of tiles in pixels
        publicConfig.oneFrameTime = config::oneFrameTime; // Duration of a single frame
        publicConfig.frameCycle = config::frameCycle; // Number of frames in one cycle
        publicConfig.maximumNumberOfLayers = config::maximumNumberOfLayers;// Maximum number of layers allowed in the game
        publicConfig.mapFirstEntity = config::mapFirstEntity; // First entity ID in the map
        publicConfig.numberOfMapEntities = config::numberOfMapEntities; // Total number of map entities
        publicConfig.enemyFirstEntity = config::enemyFirstEntity; // First entity ID for enemies
        publicConfig.numberOfEnemyEntities = config::numberOfEnemyEntities; // Number of enemies allowed in the game

        // Game settings
        publicConfig.backgroundColor = config::backgroundColor; // Background color of the game in hex code
        publicConfig.applyKnockback = config::applyKnockback; // Whether knockback effects are applied

        // Dungeon settings
        publicConfig.maxDungeonDepth = config::maxDungeonDepth; // Maximum depth of dungeons
        publicConfig.startingPosition = config::startingPosition; // Player's starting position
        publicConfig.spawnOffset = config::spawnOffset; // Offset for enemy spawn positions

        // Player stats
        publicConfig.playerAttackRange = config::playerAttackRange; // Player's attack range
        publicConfig.playerAttackDamage = config::playerAttackDamage; // Damage dealt by player's attack
        publicConfig.playerAttackAngle = config::playerAttackAngle; // Angle of player's attack
        publicConfig.playerAnimation = config::playerAnimation; // Player's animation index
        publicConfig.playerAcc = config::playerAcc; // Player's acceleration
        publicConfig.maxCharacterHP = config::maxCharacterHP; // Maximum health points for the player
        publicConfig.defaultCharacterHP = config::defaultCharacterHP; // Default health points for the player
        publicConfig.invulnerabilityTimeAfterDMG = config::invulnerabilityTimeAfterDMG;// Invulnerability time after taking damage

        // Enemy stats
        publicConfig.enemyAcc = config::enemyAcc; // Enemy acceleration
        publicConfig.defaultEnemyKnockbackForce = config::defaultEnemyKnockbackForce;// Knockback force applied to enemies

        // Text tag settings
        publicConfig.textTagDefaultSize = config::textTagDefaultSize; // Default size of text tags
        publicConfig.textTagDefaultLifetime = config::textTagDefaultLifetime; // Lifetime of text tags
        publicConfig.textTagDefaultSpeed = config::textTagDefaultSpeed; // Speed of text tags
        publicConfig.textTagDefaultAcceleration = config::textTagDefaultAcceleration; // Acceleration of text tags
        publicConfig.textTagDefaultFadeValue = config::textTagDefaultFadeValue; // Fade value of text tags

        // Weapon settings
        publicConfig.weaponComponentDefaultDamageAmount = config::weaponComponentDefaultDamageAmount;// Default damage amount for weapons
        publicConfig.weaponComponentDefaultIsAttacking = config::weaponComponentDefaultIsAttacking;// Whether the weapon is currently attacking
        publicConfig.weaponComponentDefaultQueuedAttack = config::weaponComponentDefaultQueuedAttack;// Queued attack status
        publicConfig.weaponComponentDefaultQueuedAttackFlag = config::weaponComponentDefaultQueuedAttackFlag;// Queued attack flag
        publicConfig.weaponComponentDefaultIsSwingingForward = config::weaponComponentDefaultIsSwingingForward;// Whether the weapon is swinging forward
        publicConfig.weaponComponentDefaultIsFacingRight = config::weaponComponentDefaultIsFacingRight;// Whether the weapon is facing right
        publicConfig.weaponComponentDefaultCurrentAngle = config::weaponComponentDefaultCurrentAngle;// Current angle of the weapon
        publicConfig.weaponComponentDefaultInitialAngle = config::weaponComponentDefaultInitialAngle;// Initial angle of the weapon
        publicConfig.weaponComponentDefaultRotationSpeed = config::weaponComponentDefaultRotationSpeed;// Weapon rotation speed
        publicConfig.weaponComponentDefaultSwingDistance = config::weaponComponentDefaultSwingDistance;// Swing distance of the weapon
        publicConfig.weaponComponentDefaultRemainingDistance = config::weaponComponentDefaultRemainingDistance;// Remaining swing distance
        publicConfig.weaponComponentDefaultRecoilAmount = config::weaponComponentDefaultRecoilAmount;// Amount of recoil caused by the weapon
        publicConfig.weaponInteractionDistance = config::weaponInteractionDistance; // Weapon interaction distance

        // Health bar settings
        publicConfig.fullHPColor = config::fullHPColor; // Color for full health
        publicConfig.lowHPColor = config::lowHPColor; // Color for low health

        // Enemy and item data
        publicConfig.enemyData = config::enemyData; // Default enemy data
        publicConfig.itemsData = config::itemsData; // Default item data

        // @formatter:on — enable formatter after this line
    }

    template <typename T>
    T getOrDefault(const json& j, const std::string& key, const T& defaultValue)
    {
        return j.contains(key) ? j[key].get<T>() : defaultValue;
    }

    void parseJsonConfig(const json& j)
    {
        // @formatter:off — disable formatter after this line

        // Engine settings
        publicConfig.debugMode = getOrDefault(j, "debugMode", config::debugMode); // Enables or disables debug mode
        publicConfig.gameScale = getOrDefault(j, "gameScale", config::gameScale); // Scale of the game (affects all visuals)
        publicConfig.meterToPixelRatio = getOrDefault(j, "meterToPixelRatio", config::meterToPixelRatio); // Conversion ratio for Box2D physics
        publicConfig.pixelToMeterRatio = getOrDefault(j, "pixelToMeterRatio", config::pixelToMeterRatio); // Conversion ratio for game to physics
        publicConfig.tileHeight = getOrDefault(j, "tileHeight", config::tileHeight); // Height of tiles in pixels
        publicConfig.oneFrameTime = getOrDefault(j, "oneFrameTime", config::oneFrameTime); // Duration of a single frame
        publicConfig.frameCycle = getOrDefault(j, "frameCycle", config::frameCycle); // Number of frames in one cycle
        publicConfig.maximumNumberOfLayers = getOrDefault(j, "maximumNumberOfLayers", config::maximumNumberOfLayers); // Maximum number of layers allowed in the game
        publicConfig.mapFirstEntity = getOrDefault(j, "mapFirstEntity", config::mapFirstEntity); // First entity ID in the map
        publicConfig.numberOfMapEntities = getOrDefault(j, "numberOfMapEntities", config::numberOfMapEntities); // Total number of map entities
        publicConfig.enemyFirstEntity = getOrDefault(j, "enemyFirstEntity", config::enemyFirstEntity); // First entity ID for enemies
        publicConfig.numberOfEnemyEntities = getOrDefault(j, "numberOfEnemyEntities", config::numberOfEnemyEntities); // Number of enemies allowed in the game

        // Game settings
        publicConfig.backgroundColor = getOrDefault(j, "backgroundColor", config::backgroundColor); // Background color of the game in hex code
        publicConfig.applyKnockback = getOrDefault(j, "applyKnockback", config::applyKnockback); // Whether knockback effects are applied

        // Dungeon settings
        publicConfig.maxDungeonDepth = getOrDefault(j, "maxDungeonDepth", config::maxDungeonDepth); // Maximum depth of dungeons
        publicConfig.startingPosition = getVec2FromJson(j, "startingPosition", config::startingPosition); // Player's starting position
        publicConfig.spawnOffset = getOrDefault(j, "spawnOffset", config::spawnOffset); // Offset for enemy spawn positions

        // Player stats
        publicConfig.playerAttackRange = getOrDefault(j, "playerAttackRange", config::playerAttackRange); // Player's attack range
        publicConfig.playerAttackDamage = getOrDefault(j, "playerAttackDamage", config::playerAttackDamage); // Damage dealt by player's attack
        publicConfig.playerAttackAngle = getOrDefault(j, "playerAttackAngle", config::playerAttackAngle); // Angle of player's attack
        publicConfig.playerAnimation = getOrDefault(j, "playerAnimation", config::playerAnimation); // Player's animation index
        publicConfig.playerAcc = getOrDefault(j, "playerAcc", config::playerAcc); // Player's acceleration
        publicConfig.maxCharacterHP = getOrDefault(j, "maxCharacterHP", config::maxCharacterHP); // Maximum health points for the player
        publicConfig.defaultCharacterHP = getOrDefault(j, "defaultCharacterHP", config::defaultCharacterHP); // Default health points for the player
        publicConfig.invulnerabilityTimeAfterDMG = getOrDefault(j, "invulnerabilityTimeAfterDMG", config::invulnerabilityTimeAfterDMG); // Invulnerability time after taking damage

        // Enemy stats
        publicConfig.enemyAcc = getOrDefault(j, "enemyAcc", config::enemyAcc); // Enemy acceleration
        publicConfig.defaultEnemyKnockbackForce = getOrDefault(j, "defaultEnemyKnockbackForce", config::defaultEnemyKnockbackForce); // Knockback force applied to enemies

        // Text tag settings
        publicConfig.textTagDefaultSize = getOrDefault(j, "textTagDefaultSize", config::textTagDefaultSize); // Default size of text tags
        publicConfig.textTagDefaultLifetime = getOrDefault(j, "textTagDefaultLifetime", config::textTagDefaultLifetime); // Lifetime of text tags
        publicConfig.textTagDefaultSpeed = getOrDefault(j, "textTagDefaultSpeed", config::textTagDefaultSpeed); // Speed of text tags
        publicConfig.textTagDefaultAcceleration = getOrDefault(j, "textTagDefaultAcceleration", config::textTagDefaultAcceleration); // Acceleration of text tags
        publicConfig.textTagDefaultFadeValue = getOrDefault(j, "textTagDefaultFadeValue", config::textTagDefaultFadeValue); // Fade value of text tags

        // Weapon settings
        publicConfig.weaponComponentDefaultDamageAmount = getOrDefault(j, "weaponComponentDefaultDamageAmount", config::weaponComponentDefaultDamageAmount); // Default damage amount for weapons
        publicConfig.weaponComponentDefaultIsAttacking = getOrDefault(j, "weaponComponentDefaultIsAttacking", config::weaponComponentDefaultIsAttacking); // Whether the weapon is currently attacking
        publicConfig.weaponComponentDefaultQueuedAttack = getOrDefault(j, "weaponComponentDefaultQueuedAttack", config::weaponComponentDefaultQueuedAttack); // Queued attack status
        publicConfig.weaponComponentDefaultQueuedAttackFlag = getOrDefault(j, "weaponComponentDefaultQueuedAttackFlag", config::weaponComponentDefaultQueuedAttackFlag); // Queued attack flag
        publicConfig.weaponComponentDefaultIsSwingingForward = getOrDefault(j, "weaponComponentDefaultIsSwingingForward", config::weaponComponentDefaultIsSwingingForward); // Whether the weapon is swinging forward
        publicConfig.weaponComponentDefaultIsFacingRight = getOrDefault(j, "weaponComponentDefaultIsFacingRight", config::weaponComponentDefaultIsFacingRight); // Whether the weapon is facing right
        publicConfig.weaponComponentDefaultCurrentAngle = getOrDefault(j, "weaponComponentDefaultCurrentAngle", config::weaponComponentDefaultCurrentAngle); // Current angle of the weapon
        publicConfig.weaponComponentDefaultInitialAngle = getOrDefault(j, "weaponComponentDefaultInitialAngle", config::weaponComponentDefaultInitialAngle); // Initial angle of the weapon
        publicConfig.weaponComponentDefaultRotationSpeed = getOrDefault(j, "weaponComponentDefaultRotationSpeed", config::weaponComponentDefaultRotationSpeed); // Weapon rotation speed
        publicConfig.weaponComponentDefaultSwingDistance = getOrDefault(j, "weaponComponentDefaultSwingDistance", config::weaponComponentDefaultSwingDistance); // Swing distance of the weapon
        publicConfig.weaponComponentDefaultRemainingDistance = getOrDefault(j, "weaponComponentDefaultRemainingDistance", config::weaponComponentDefaultRemainingDistance); // Remaining swing distance
        publicConfig.weaponComponentDefaultRecoilAmount = getOrDefault(j, "weaponComponentDefaultRecoilAmount", config::weaponComponentDefaultRecoilAmount); // Amount of recoil caused by the weapon
        publicConfig.weaponInteractionDistance = getOrDefault(j, "weaponInteractionDistance", config::weaponInteractionDistance); // Weapon interaction distance

        // Health bar settings
        publicConfig.fullHPColor = getImVec4FromJson(j, "fullHPColor", config::fullHPColor); // Color for full health
        publicConfig.lowHPColor = getImVec4FromJson(j, "lowHPColor", config::lowHPColor); // Color for low health

        
        // Helper function to parse TileComponent
        auto parseTileComponent = [](const json& tileJson) -> TileComponent {
            return TileComponent{
                tileJson.value("tileID", 0u),
                tileJson.value("tileSet", ""),
                tileJson.value("tileLayer", 0)
            };
        };

        // Helper function to parse Collision
        auto parseCollision = [](const json& collisionJson) -> Collision {
            return Collision{
                collisionJson.value("type", 0),
                collisionJson.value("width", 0.f),
                collisionJson.value("height", 0.f),
                collisionJson.value("xOffset", 0.f),
                collisionJson.value("yOffset", 0.f)
            };
        };

        // Parse enemyData
        if (j.contains("enemyData") && j["enemyData"].is_array()) {
            publicConfig.enemyData.clear(); 
            for (const auto& enemy : j["enemyData"]) {
                std::string typeStr = enemy.value("type", ""); 
                Enemies::EnemyType type = Enemies::enemyTypeMap.at(typeStr);

                config::EnemyConfig config{
                    enemy.value("name", ""),
                    enemy.value("hp", 0.f),
                    enemy.value("damage", 0.f),
                    parseTileComponent(enemy["textureData"]),
                    parseCollision(enemy["collisionData"])
                };
                publicConfig.enemyData.insert({type, config});
            }
        }

        // Parse itemsData
        if (j.contains("itemsData") && j["itemsData"].is_array()) {
            publicConfig.itemsData.clear();
            for (const auto& item : j["itemsData"]) {
                std::string behaviourStr = item.value("behaviour", ""); 
                Items::Behaviours behaviour = Items::behaviourMap.at(behaviourStr);
        
                config::ItemConfig itemConfig{
                    item.value("name", ""),
                    item.value("value", 0.f),
                    behaviour,
                    parseTileComponent(item["textureData"])
                };
                publicConfig.itemsData.push_back(itemConfig);
            }
        }

        // @formatter:on — enable formatter after this line
    }

    ImVec4 getImVec4FromJson(const json& j, const std::string& key, const ImVec4& defaultValue)
    {
        if (j.contains(key) && j[key].is_array() && j[key].size() == 4)
        {
            return ImVec4(
                j[key][0].get<float>(),
                j[key][1].get<float>(),
                j[key][2].get<float>(),
                j[key][3].get<float>()
                );
        }
        return defaultValue;
    }

    glm::vec2 getVec2FromJson(const json& j, const std::string& key, const glm::vec2& defaultValue)
    {
        if (j.contains(key) && j[key].is_array() && j[key].size() == 2)
        {
            return glm::vec2(
                j[key][0].get<float>(),
                j[key][1].get<float>()
                );
        }
        return defaultValue;
    }
};