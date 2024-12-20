#pragma once
#include <imgui.h>
#include <unordered_map>

#include "System.h"
#include "SFML/Graphics/Texture.hpp"

class ButtonSystem : public System
{
public:
    void render();
    void loadData(const std::string& atlasPath, const std::string& fontPath, float fontSize);

private:
    std::unordered_map<std::string, sf::Texture> m_loadedTextures;
    std::unordered_map<std::string, ImFont*> m_loadedFonts;
};