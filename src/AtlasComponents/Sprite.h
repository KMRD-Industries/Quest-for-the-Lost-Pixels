#pragma once

#ifndef SPRITE_H
#define SPRITE_H

#include <SFML/Graphics.hpp>
#include "Texture.h"

namespace moony{
    struct Sprite : sf::Transformable{
	Sprite();
	Sprite(const Texture&, int, sf::Color = sf::Color::White);

	Texture m_subtexture;
	sf::Color m_color;
	int m_layer;
    };
}

#endif