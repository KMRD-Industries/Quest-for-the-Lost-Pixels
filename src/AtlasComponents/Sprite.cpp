//
// Created by dominiq on 3/20/24.
//

#include "Sprite.h"

moony::Sprite::Sprite(){
    m_layer = 0;
    m_color = sf::Color::White;
}

moony::Sprite::Sprite(const Texture& subtexture, int layer = 0, sf::Color color){
    m_subtexture = subtexture;
    m_layer = layer;
    m_color = color;
}
