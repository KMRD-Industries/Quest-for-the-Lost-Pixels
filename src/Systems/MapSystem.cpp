#include "MapSystem.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "AnimationComponent.h"
#include "Coordinator.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "TileComponent.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void MapSystem::draw(sf::RenderWindow& window) const {

//    for(int i = 0; i < 4; i++) {
        // 4 - Max layer number:
        // Draw entities in layer order
        for (const auto& entity : m_entities)
        {
            auto& mapComponent = gCoordinator.getComponent<TileComponent>(entity);
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
            auto& animationComponent = gCoordinator.getComponent<AnimationComponent>(entity);

            if (mapComponent.id == 0) continue;

            // Calculate next frame of animation
            if (!animationComponent.frames.empty()){
                // load next animation frame
                animationComponent.ignoreframes++;
                if (animationComponent.ignoreframes % 20 == 0)
                    animationComponent.actual = (animationComponent.actual + 1) % animationComponent.frames.size();

                mapComponent.id = animationComponent.frames[animationComponent.actual];
            }

//            if(mapComponent.layer == i)
            window.draw(createTile(mapComponent.id, transformComponent.position, transformComponent.rotation, transformComponent.scale));
        }
//    }
}

sf::Sprite MapSystem::createTile(uint32_t id, const sf::Vector2f& position, const float& rotation, const sf::Vector2f& scale) const {
    const auto& texture = texture_atlas.findSubTexture(id - 1);
    sf::Sprite new_sprite(*texture.m_texture, texture.m_rect);

    new_sprite.setOrigin(new_sprite.getLocalBounds().width / 2, new_sprite.getLocalBounds().height / 2);
    float scalar = 3.f;

    new_sprite.setScale(scale * scalar);
    new_sprite.setRotation(new_sprite.getRotation() + rotation);
    new_sprite.setPosition(position.x, position.y);

    return new_sprite;
}

void MapSystem::loadMap(std::string& path) {
    // Reset Map Entities to default

    for (const auto& entity : m_entities){
        auto& mapComponent = gCoordinator.getComponent<TileComponent>(entity);
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        auto& animationComponent = gCoordinator.getComponent<AnimationComponent>(entity);

        mapComponent.id = 0;
        animationComponent.frames.clear();
        transformComponent.scale = sf::Vector2f(1.f, 1.f);
        transformComponent.rotation = 0.f;
    }

    std::ifstream jsonFile(path);

    if (!jsonFile.is_open()) {
        return;
    }

    std::unordered_map<std::string, uint32_t> atlas_sets;
    nlohmann::json parsedFile = nlohmann::json::parse(jsonFile);

    for(auto tileset: parsedFile["tilesets"]){
        std::string name = extractFileName(tileset["source"], "/", ".");
        uint32_t first_gid = tileset["firstgid"];

        atlas_sets[name] = first_gid;
    }

    auto startIterator = m_entities.begin();
    startIterator++;
    int layer = 0;

    uint32_t width = 0;
    uint32_t height = 0;

    for (auto & data : parsedFile["layers"]){
        static const std::uint32_t mask = 0xf0000000;
        int index = 0;
        layer++;

        // TODO: Objects
        if (!data.contains("data"))
            continue;

        width = static_cast<unsigned long>(data["width"]);
        height = static_cast<unsigned long>(data["height"]);


        for (uint32_t i : processDataString(data["data"],width * height,0)) {

            uint32_t flipFlags =  (i & mask) >> 28;
            uint32_t tileID = i & ~mask;

            int xPosition = index % (static_cast<int>(width));
            int yPosition = index / (static_cast<int>(width));

            if(tileID >= 1){
                auto& tileComponent = gCoordinator.getComponent<TileComponent>(*startIterator);
                auto& transformMapComponent = gCoordinator.getComponent<TransformComponent>(*startIterator);
                auto& animationComponent = gCoordinator.getComponent<AnimationComponent>(*startIterator);

                std::string tileset_name = findKeyLessThan(atlas_sets, tileID);
                tileID = std::abs(long(this->texture_atlas.getFirstGidOfSet(tileset_name))) + (tileID - atlas_sets[tileset_name])  + 1;

                tileComponent.id = tileID;
                tileComponent.layer = layer;

                if(this->texture_atlas.map_animations.contains(tileID)){
                    animationComponent.frames = this->texture_atlas.map_animations[tileID];
                }

                transformMapComponent.position = sf::Vector2f(static_cast<float>(xPosition), static_cast<float>(yPosition)) * 16.f * 3.f;
                doFlips(flipFlags, transformMapComponent.rotation, transformMapComponent.scale);

                startIterator++;
            }
            index++;
        }
    }
}
void MapSystem::doFlips(std::uint8_t flags, float& rotation, sf::Vector2f& scale) {
    //0000 = no change
    //0100 = vertical = swap y axis
    //1000 = horizontal = swap x axis
    //1100 = horiz + vert = swap both axes = horiz+vert = rotate 180 degrees
    //0010 = diag = rotate 90 degrees right and swap x axis
    //0110 = diag+vert = rotate 270 degrees right
    //1010 = horiz+diag = rotate 90 degrees right
    //1110 = horiz+vert+diag = rotate 90 degrees right and swap y axis

    // Horizontal = 0x8;
    // Vertical = 0x4;
    // Diagonal = 0x2;

    switch(flags) {
        case 0x4: // 0100 = vertical flip
            scale.y *= -1;
            break;
        case 0x8: // 1000 = horizontal flip
            scale.x *= -1;
            break;
        case 0xC: // 1100 = horizontal and vertical flip
            scale = -scale;
            break;
        case 0x2: // 0010 = diagonal flip
            scale.x *= -1;
            rotation += 90.f;
            break;
        case 0x6: // 0110 = diagonal and vertical flip
            rotation += 270.f;
            break;
        case 0xA: // 1010 = diagonal and horizontal flip
            rotation += 90.f;
            break;
        case 0xE: // 1110 = diagonal, horizontal, and vertical flip
            rotation += 90.f;
            scale.x *= -1;
            break;
        default:
            // No flip, do nothing
            break;
    }
}

std::string MapSystem::findKeyLessThan(const std::unordered_map<std::string, uint32_t>& atlas_sets, uint32_t i) {
    std::string result;
    for (const auto& pair : atlas_sets) {
        if (pair.second <= i) {
            result = pair.first;
            break;
        }
    }
    return result;
}