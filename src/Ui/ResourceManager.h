#pragma once

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <string>
#include <unordered_map>

class ResourceManager
{
public:
    static ResourceManager& getInstance()
    {
        static ResourceManager instance;
        return instance;
    }

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    sf::Texture& getTexture(const std::string& texturePath)
    {
        if (!m_loadedFonts.contains(texturePath))
        {
            sf::Texture texture;
            if (!texture.loadFromFile(texturePath))
                std::cerr << "Error loading texture from: " << texturePath << std::endl;
            else
                m_loadedTextures[texturePath] = std::move(texture);
        }
        return m_loadedTextures[texturePath];
    }

    ImFont* getFont(const std::string& fontPath, const float fontSize)
    {
        const std::string fontKey = fontPath + std::to_string(fontSize);
        if (!m_loadedFonts.contains(fontKey))
        {
            ImGuiIO& io = ImGui::GetIO();
            ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize);
            if (!font)
                std::cerr << "Error loading font from: " << fontPath << std::endl;
            else
                m_loadedFonts[fontKey] = font;
            ImGui::SFML::UpdateFontTexture();
        }
        return m_loadedFonts[fontKey];
    }

    void setIP(const std::string& serverIp) { m_serverIp = serverIp; }

    std::string getIP() { return m_serverIp; }

private:
    ResourceManager() = default;

    std::unordered_map<std::string, sf::Texture> m_loadedTextures;
    std::unordered_map<std::string, ImFont*> m_loadedFonts;
    std::string m_serverIp{"127.0.0.1:9001"};
};