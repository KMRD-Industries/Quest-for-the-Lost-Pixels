//
// Created by dominiq on 3/20/24.
//

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

    uint32_t gid = 0;

    if(!m_atlas_list.empty())
        gid = m_atlas_list.back().first_gid + m_atlas_list.size() + 1;

    m_atlas_list.emplace_back(gid);

    gid = m_atlas_list.back().first_gid;
    sf::Texture tex;
    tex.loadFromImage(image);
    m_atlas_list.back().m_texture = std::make_unique<sf::Texture>(tex);

    for (unsigned int y = 0; y < height; y += 16) {
        for (unsigned int x = 0; x < width; x += 16){
            // Create a texture for the current tile
            sf::Texture texture;
            sf::IntRect texture_rect = sf::IntRect(x, y, 16, 16);
            texture.loadFromImage(image, texture_rect);

            // Create a sprite for the current tile
            sf::Sprite sprite(texture);
            m_atlas_list.back().m_texture_table[gid++] = texture_rect;
        }
    }

    return 1;
}

Texture TextureAtlas::findSubTexture(uint32_t id) const {
    for (const auto& atlas : m_atlas_list) { // Marking atlas as const
        if (atlas.m_texture_table.contains(id)) {
            // Instead of returning a temporary Texture object, construct it using std::make_shared
            return Texture(atlas.m_texture, atlas.m_texture_table.at(id));
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
