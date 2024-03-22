#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <SFML/Graphics.hpp>


namespace moony
{
	// The Texture class is similar to the SFML sf::Texture class but was designed to work specifically with the TextureAtlas class.
	struct Texture{
	    explicit Texture(const sf::Texture* = nullptr, sf::IntRect = sf::IntRect());

		const sf::Texture* m_texture;
		sf::IntRect m_rect;
	};
}


#endif