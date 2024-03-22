#pragma once

#ifndef SPRITEBATCH_H
#define SPRITEBATCH_H

// Number of frames a batch can be empty for before it is destroyed

#include <vector>
#include <array>
#include <random>
#include <iostream>
#include <SFML/Graphics.hpp>

#include "Texture.h"
#include "Sprite.h"


namespace moony {
class SpriteBatch final : public sf::Drawable{
public:
    void clear();
    void draw(const Sprite& sprite);
    void order();

private:
    struct Batch{
        void clear();
        void draw(const Sprite&);

        const sf::Texture* m_texture;
        int m_layer;
        unsigned int m_inactive;
        unsigned int m_count;
        std::vector<sf::Vertex> m_vertices;
    };

    Batch* findBatch(const Sprite& sprite);
    void draw(sf::RenderTarget& target, sf::RenderStates state) const override;

    std::vector<Batch*> m_batches;
};
}

#endif