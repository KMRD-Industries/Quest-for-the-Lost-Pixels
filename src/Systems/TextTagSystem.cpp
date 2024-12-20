#include "TextTagSystem.h"
#include <iostream>
#include "AnimationSystem.h"
#include "Config.h"
#include "GameUtility.h"
#include "Paths.h"
#include "TextTagComponent.h"
#include "TransformComponent.h"

extern PublicConfigSingleton configSingleton;

TextTagSystem::TextTagSystem() { this->loadFont(std::string(ASSET_PATH) + "/fonts/PixellettersFull.ttf"); }

void TextTagSystem::performFixedUpdate() const
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
}

void TextTagSystem::update(const float& deltaTime)
{
    if (m_frameTime += deltaTime; m_frameTime >= config::oneFrameTimeMs)
    {
        m_frameTime -= config::oneFrameTimeMs;
        performFixedUpdate();
        deleteTags();
    }
}

void TextTagSystem::loadFont(const std::string& path)
{
    if (!this->font.loadFromFile(path))
        std::cout << "ERROR::TEXT_TAG_SYSTEM::CONSTRUCTOR::Failed to load font " << path << "\n";
}

void TextTagSystem::render(sf::RenderTarget& window)
{
    for (const auto& entity : m_entities)
    {
        auto& textTag = gCoordinator.getComponent<TextTagComponent>(entity);
        const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        textTag.text.setPosition(transformComponent.position + GameUtility::mapOffset);
        textTag.text.setString(std::to_string(configSingleton.GetConfig().playerAttackDamage));
        textTag.text.setFillColor(textTag.color);
        textTag.text.setScale(configSingleton.GetConfig().gameScale, configSingleton.GetConfig().gameScale);
        textTag.text.setCharacterSize(15);

        window.draw(textTag.text);
    }
}

void TextTagSystem::deleteTags()
{
    std::deque<Entity> entityToRemove;

    for (const auto entity : m_entities)
    {
        const auto& textTag = gCoordinator.getComponent<TextTagComponent>(entity);
        if (textTag.lifetime <= 0) entityToRemove.push_back(entity);
    }

    while (!entityToRemove.empty())
    {
        gCoordinator.destroyEntity(entityToRemove.front());
        entityToRemove.pop_front();
    }
}