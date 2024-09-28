#pragma once
#include <unordered_map>

#include "System.h"

struct ImFont;

namespace sf
{
    class Texture;
}

class ButtonSystem : public System
{
public:
    void render();
    void loadData();

private:
    std::unordered_map<std::string, sf::Texture> m_loadedTextures;
    std::unordered_map<std::string, ImFont*> m_loadedFonts;
};