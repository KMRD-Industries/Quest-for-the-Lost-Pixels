#pragma once

#include "TextureAtlas.h"
#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>

/**
 * This is to load TileSet from json file to atlas.
 * @param file_path path to TileSet
 * @return
 */
int TextureAtlas::loadFromFile(const std::string& file_path) {
    std::ifstream jsonFile(file_path);

    if (!jsonFile.is_open()) {
        std::cout << "Failed to load TileSet." << std::endl;
        return 0;
    }

    nlohmann::json parsedFile = nlohmann::json::parse(jsonFile);
    std::string image_path = parsedFile["image"];

    sf::Image image;

    // TODO FIX IT
    if (!image.loadFromFile("../../resources/TileSets/" + image_path)){
        std::cout << "Failed to load image." << std::endl;
        return 0;
    }

    unsigned int width = parsedFile["imagewidth"];
    unsigned int height = parsedFile["imageheight"];
    int tileWidth = parsedFile["tilewidth"];
    int tileHeight = parsedFile["tileheight"];

    uint32_t gid = getFirstUnusedGid();
    std::string tileset_name = extractFileName(file_path, "/", ".");

    // Add new Atlas struct with FIRST unused tile id
    m_atlas_list.emplace_back(gid);
    m_atlas_map[tileset_name] = gid;

    uint32_t fist_gid_copy = gid;
    gid = m_atlas_list.back().first_gid;

    sf::Texture tex;
    tex.loadFromImage(image);
    m_atlas_list.back().m_texture = std::make_unique<sf::Texture>(tex);

    for (int y = 0; y < height; y += tileHeight) {
        for (int x = 0; x < width; x += tileWidth){
            m_atlas_list.back().m_texture_table[gid++] =  sf::IntRect(x, y, tileHeight, tileHeight);
        }
    }

    // Animation (MAP ONLY)
    auto it = parsedFile.find("tiles");
    if (it != parsedFile.end()) {
        auto& tilesArray = parsedFile["tiles"];
        for (auto& tile : tilesArray) {
            auto& animationArray = tile["animation"];
            std::vector<uint32_t> frames;

            for (auto& frame : animationArray) {
                uint32_t tileid = frame["tileid"];
                uint32_t adjusted_id = tileid + fist_gid_copy + 1;

                frames.push_back(adjusted_id);
            }

            if (!frames.empty()) {
                this->map_animations[frames.at(0)] = frames;
            }
        }
    }
    return 1;
}

Texture TextureAtlas::findSubTexture(uint32_t tile_id) const {
    for (const auto& atlas : m_atlas_list) {
        if (atlas.m_texture_table.contains(tile_id)) {
            return Texture(atlas.m_texture, atlas.m_texture_table.at(tile_id));
        }
    }
    // If no subtexture with the given id is found, return an empty Texture
    return Texture();
}

// This function gets a list of all the separate image names that are in the TextureAtlas.
// Returns a std::vector<std::string> full of filenames corresponding to images in the TextureAtlas.
std::vector<uint32_t> TextureAtlas::getSubTextureIds(){
    std::vector<uint32_t> ids;

    for(const auto& atlas : m_atlas_list)
	for(const auto& [string, rect] : atlas.m_texture_table)
	    ids.push_back(string);

    return ids;
}

// Return First Tile ID in tileset
uint32_t TextureAtlas::getFirstGidOfSet(const std::string& name) {
    if(this->m_atlas_map.contains(name))
        return this->m_atlas_map.at(name);

    return 1;
}

uint32_t TextureAtlas::getFirstUnusedGid() {
    return (!m_atlas_list.empty()) ? m_atlas_list.back().first_gid + m_atlas_list.back().m_texture_table.size() : 1;
}


