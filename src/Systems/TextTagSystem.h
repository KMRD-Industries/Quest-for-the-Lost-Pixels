#pragma once

#include <unordered_map>
#include "SFML/Graphics/Font.hpp"
#include "System.h"


struct TextTag;
class TextTagSystem : public System
{
public:
    void init();
    void loadFont(const std::string&);
    void initPresets();
    void render(sf::RenderTarget&);
    void deleteTags();
    void update();

    enum TextTagPresets : int
    {
        DAMAGE_TAKEN = 0
    };

private:
    sf::Font font;
};
