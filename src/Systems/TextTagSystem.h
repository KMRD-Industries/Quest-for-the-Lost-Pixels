#pragma once

#include "SFML/Graphics/Font.hpp"
#include "System.h"

struct TextTag;

class TextTagSystem : public System
{
public:
    TextTagSystem();
    void update(const float &deltaTime);
    void render(sf::RenderTarget &);

private:
    void deleteTags();
    void loadFont(const std::string &);
    void performFixedUpdate() const;
    sf::Font font;
    float m_frameTime{};
};