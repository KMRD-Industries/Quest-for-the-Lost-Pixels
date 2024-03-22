//
// Created by dominiq on 3/20/24.
//

#include "SpriteBatch.h"

void moony::SpriteBatch::clear() {
    for(unsigned int i = 0; i < m_batches.size(); i++){
        m_batches[i]->clear();

        if(m_batches[i]->m_inactive > 256){
            std::swap(m_batches[i], m_batches.back());
            delete m_batches.back();
            m_batches.pop_back();
            i--;
        }
    }
}

moony::SpriteBatch::Batch* moony::SpriteBatch::findBatch(const Sprite& sprite){
    for(Batch* batch : m_batches){
        if(batch->m_texture == sprite.m_subtexture.m_texture && batch->m_layer == sprite.m_layer)
            return batch;
    }

    auto* batch = new Batch();
    batch->m_texture = sprite.m_subtexture.m_texture;
    batch->m_layer = sprite.m_layer;
    batch->m_inactive = 0;
    batch->m_count = 0;

    m_batches.push_back(batch);
    return m_batches.back();
}


void moony::SpriteBatch::draw(const Sprite& sprite){
    if(sprite.m_subtexture.m_texture == nullptr)
        return;

    findBatch(sprite)->draw(sprite);
}

void moony::SpriteBatch::order(){

    // Sort the batches
    std::ranges::sort(m_batches.begin(), m_batches.end(), [&](const Batch* a_batch, const Batch* b_batch){
            if(a_batch->m_layer == b_batch->m_layer)
                return a_batch->m_texture < b_batch->m_texture;

        return a_batch->m_layer < b_batch->m_layer;
    });
}

void moony::SpriteBatch::Batch::clear() {
    if(m_count > 0) {
        m_count = 0;
        m_inactive = 0;
    }
    else
        m_inactive++;
}

void moony::SpriteBatch::Batch::draw(const moony::Sprite& sprite) {
    size_t vertex_count = m_count * 4;

    if(vertex_count + 4 > m_vertices.size())
        m_vertices.resize(m_vertices.size() + 4);

    sf::Vector2f position(sprite.m_subtexture.m_rect.left, sprite.m_subtexture.m_rect.top);
    sf::Vector2f size(sprite.m_subtexture.m_rect.width, sprite.m_subtexture.m_rect.height);

    sf::Transform transform = sprite.getTransform();

    m_vertices[vertex_count].position = transform.transformPoint(0.0f, 0.0f);
    m_vertices[vertex_count].texCoords = position;
    m_vertices[vertex_count].color = sprite.m_color;

    m_vertices[vertex_count + 1].position = transform.transformPoint(0.0f, size.y);
    m_vertices[vertex_count + 1].texCoords = sf::Vector2f(position.x, size.y + position.y);
    m_vertices[vertex_count + 1].color = sprite.m_color;

    m_vertices[vertex_count + 2].position = transform.transformPoint(size);
    m_vertices[vertex_count + 2].texCoords = size + position;
    m_vertices[vertex_count + 2].color = sprite.m_color;

    m_vertices[vertex_count + 3].position = transform.transformPoint(size.x, 0.0f);
    m_vertices[vertex_count + 3].texCoords = sf::Vector2f(size.x + position.x, position.y);
    m_vertices[vertex_count + 3].color = sprite.m_color;

    m_count++;
}


void moony::SpriteBatch::draw(sf::RenderTarget& target, sf::RenderStates states) const{
    for(const Batch* batch : m_batches)
    {
        states.texture = batch->m_texture;
        target.draw(batch->m_vertices.data(), batch->m_count * 4, sf::PrimitiveType::Quads, states);
    }
}


