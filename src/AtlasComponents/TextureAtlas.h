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


namespace moony
{
	// The TextureAtlas class holds all the Moony Texture objects inside Atlas objects which can be 
	// referenced by the name of the original file they were loaded as. Only one of these objects should be necessary.
	class TextureAtlas{
	public:
		bool loadFromFile(const std::string& file_path);
		Texture findSubTexture(const std::string& name);
		[[nodiscard]] std::vector<std::string> getSubTextureNames() const;

	private:
		struct Atlas{
			Atlas() : m_texture(new sf::Texture) {};
			std::unique_ptr<sf::Texture> m_texture;
			std::unordered_map<std::string, sf::IntRect> m_texture_table;
		};

		std::vector<Atlas> m_atlas_list;
	};


}

#endif