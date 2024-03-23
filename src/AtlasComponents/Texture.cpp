//
// Created by dominiq on 3/20/24.
//

#include "Texture.h"

moony::Texture::Texture(const sf::Texture* texture, const sf::IntRect rect){
    m_texture = texture;
    m_rect = rect;
}