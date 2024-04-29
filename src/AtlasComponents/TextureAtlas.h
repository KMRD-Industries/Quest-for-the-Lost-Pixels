#pragma once

#ifndef TEXTUREATLAS_H
#define TEXTUREATLAS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <SFML/Graphics.hpp>

#ifdef USE_ZLIB
	#include <zlib.h>
#endif

#include "Texture.h"
#include "Utils/Helpers.cpp"

class TextureAtlas {
public:
        int loadFromFile(const std::string& file_path);
        [[nodiscard]] std::vector<uint32_t> getSubTextureIds();
        TextureAtlas()= default;
        [[nodiscard]] Texture findSubTexture(uint32_t id) const;
        uint32_t getFirstGidOfSet(const std::string&);

       private:
        struct Atlas{
            explicit Atlas(const int gid) :first_gid(gid), m_texture(new sf::Texture) {};
            int first_gid;
            std::shared_ptr<sf::Texture> m_texture;
            std::unordered_map<uint32_t, sf::IntRect> m_texture_table;
        };

        std::vector<Atlas> m_atlas_list;
        std::unordered_map<std::string, uint32_t> m_atlas_map;
};


#endif