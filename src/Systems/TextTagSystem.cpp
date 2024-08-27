#include "TextTagSystem.h"

#include <iostream>

#include "AnimationSystem.h"
#include "Config.h"
#include "Paths.h"
#include "TextTag.h"
#include "TextTagComponent.h"
#include "TileComponent.h"
#include "TransformComponent.h"

void TextTagSystem::init() { this->loadFont(std::string(ASSET_PATH) + "/fonts/PixellettersFull.ttf"); }

void TextTagSystem::update()
{
    for (const auto& entity : m_entities)
    {
        auto& textTagComponent = gCoordinator.getComponent<TextTagComponent>(entity);
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        textTagComponent.text.setFont(this->font);
        textTagComponent.text.setOutlineColor(sf::Color(0, 0, 0, 128));
        textTagComponent.text.setOutlineThickness(1);
        textTagComponent.lifetime--;
        transformComponent.position.y += textTagComponent.speed;
    }

    deleteTags();
}

void TextTagSystem::loadFont(const std::string& path)
{
    if (!this->font.loadFromFile(path))
        std::cout << "ERROR::TEXTTAGSYSTEM::CONSTRUCTOR::Failed to load font " << path << "\n";
}

void TextTagSystem::initPresets() {}

void TextTagSystem::render(sf::RenderTarget& target) {}

void TextTagSystem::deleteTags()
{
    std::deque<Entity> entityToRemove;

    for (const auto entity : m_entities)
    {
        const auto& textTag = gCoordinator.getComponent<TextTagComponent>(entity);
        if (textTag.lifetime <= 0)
        {
            entityToRemove.push_back(entity);
        }
    }

    while (!entityToRemove.empty())
    {
        gCoordinator.destroyEntity(entityToRemove.front());
        entityToRemove.pop_front();
    }
}