#pragma once

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

#include "GameTypes.h"

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

    void loadShader(const std::string& shaderPath, const video::FragmentShader shaderType)
    {
        auto shaderToLoad = std::make_shared<sf::Shader>();
        if (!shaderToLoad->loadFromFile(shaderPath, sf::Shader::Fragment))
        {
            std::cerr << "Error loading shader from: " << shaderPath << std::endl;
        }
        m_loadedShaders[shaderType] = shaderToLoad;
    }

    void setCurrentShader(const video::FragmentShader shader) { m_currentShader = shader; }

    std::shared_ptr<sf::Shader> getCurrentShader() { return m_loadedShaders[m_currentShader]; }
    video::FragmentShader getCurretShaderType() { return m_currentShader; }

private:
    ResourceManager() = default;

    video::FragmentShader m_currentShader{video::FragmentShader::NONE};
    std::unordered_map<video::FragmentShader, std::shared_ptr<sf::Shader>> m_loadedShaders;
    std::unordered_map<std::string, sf::Texture> m_loadedTextures;
    std::unordered_map<std::string, ImFont*> m_loadedFonts;
    std::string m_serverIp{"127.0.0.1:9001"};
};