#pragma once

#include "TextureAtlas.h"

#include <fstream>
#include <iostream>

int TextureAtlas::loadFromFile(const std::string& file_path) {
    sf::Image image;

    if (!image.loadFromFile(file_path)){
        std::cout << "Failed to load from atlas." << std::endl;
        return 0;
    }

    unsigned int width = image.getSize().x;
    unsigned int height = image.getSize().y;

    uint32_t gid = (!m_atlas_list.empty()) ? m_atlas_list.back().first_gid + m_atlas_list.back().m_texture_table.size() : 0;
    std::string tileset_name = extractFileName(file_path, "/", ".");

    m_atlas_list.emplace_back(gid);
    m_atlas_map[tileset_name] = gid;

    gid = m_atlas_list.back().first_gid;

    sf::Texture tex;
    tex.loadFromImage(image);
    m_atlas_list.back().m_texture = std::make_unique<sf::Texture>(tex);

    for (int y = 0; y < height; y += 16) {
        for (int x = 0; x < width; x += 16){
            m_atlas_list.back().m_texture_table[gid] =  sf::IntRect(x, y, 16, 16);
            gid++;
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

    return 0;
}
