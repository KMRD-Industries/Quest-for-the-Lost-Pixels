#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <cstdint>
#include <functional>
#include <glm/glm.hpp>
#include <imgui.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include "Config.h"
#include "GameTypes.h"
#include "glm/gtx/hash.hpp"

static inline config::EnemyConfig getRandomEnemyData(const Enemies::EnemyType &enemyType) {
    static std::mt19937 gen{std::random_device{}()};

    auto enemyConfig = config::enemyData.equal_range(enemyType);
    std::vector<config::EnemyConfig> enemiesConfig;
    for (auto it = enemyConfig.first; it != enemyConfig.second; ++it) enemiesConfig.push_back(it->second);

    if (enemiesConfig.empty()) {
        std::cerr << "[WARNING] There is not enemy for chosen type! \n";
        return config::EnemyConfig{};
    }

    std::uniform_int_distribution<int> distrib(0, enemiesConfig.size() - 1);
    const auto randomIndex{distrib(gen)};

    return enemiesConfig[randomIndex];
}

static inline config::ItemConfig getRandomItemData() {
    static std::mt19937 gen{std::random_device{}()};

    std::uniform_int_distribution<int> distrib(0, config::itemsData.size() - 1);
    const auto randomIndex{distrib(gen)};

    return config::itemsData[randomIndex];
}

static inline ImVec4 interpolateColor(const ImVec4 &color1, const ImVec4 &color2, const float t) {
    return ImVec4(color1.x + t * (color2.x - color1.x), color1.y + t * (color2.y - color1.y),
                  color1.z + t * (color2.z - color1.z), color1.w + t * (color2.w - color1.w));
}

static inline std::string base64_decode(const std::string &encoded_string) {
    static const std::string base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

    std::function<bool(unsigned char)> is_base64 = [](unsigned char c) -> bool {
        return (isalnum(c) || (c == '+') || (c == '/'));
    };

    auto in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++) char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++) ret += char_array_3[i];

            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++) char_array_4[j] = 0;


        for (j = 0; j < 4; j++) char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}

static inline std::vector<uint32_t> processDataString(std::string dataString, std::size_t tileCount,
                                                      int32_t compressionType) {
    std::stringstream ss;
    ss << dataString;
    ss >> dataString;
    dataString = base64_decode(dataString);

    std::size_t expectedSize = tileCount * 4; // 4 bytes per tile
    std::vector<unsigned char> byteData;
    byteData.reserve(expectedSize);

    byteData.insert(byteData.end(), dataString.begin(), dataString.end());

    // data stream is in bytes so we need to OR into 32 bit values
    std::vector<uint32_t> IDs;
    IDs.reserve(tileCount);
    for (auto i = 0u; i < expectedSize - 3u; i += 4u) {
        uint32_t id = byteData[i] | byteData[i + 1] << 8 | byteData[i + 2] << 16 | byteData[i + 3] << 24;
        IDs.push_back(id);
    }

    return IDs;
};

static std::string extractFileName(const std::string &filePath, const std::string &begin, const std::string &end) {
    // Find the position of the last '/'
    size_t lastSlashPos = filePath.find_last_of(begin);
    if (lastSlashPos == std::string::npos) {
        // If '/' is not found, return an empty string or handle error accordingly
        return "";
    }

    // Find the position of ".tsx"
    size_t tsxPos = filePath.find(end, lastSlashPos);
    if (tsxPos == std::string::npos) {
        // If ".tsx" is not found, return an empty string or handle error accordingly
        return "";
    }

    // Extract the substring between lastSlashPos and tsxPos
    return filePath.substr(lastSlashPos + 1, tsxPos - lastSlashPos - 1);
}

struct IVec2Compare {
    bool operator()(const glm::ivec2 &lhs, const glm::ivec2 &rhs) const {
        return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
    }
};

static std::unordered_multimap<glm::ivec2, int> findSpecialBlocks(const nlohmann::json &json) {
    std::unordered_multimap<glm::ivec2, int> result;

    const int width = json["width"];
    const int height = json["height"];

    int specialBlocksFirstGID = -1;
    int nextTilesetFirstGID = INT_MAX; // Pocz�tkowy identyfikator GID nast�pnego tilesetu

    for (const auto &tileset: json["tilesets"])
        // Sprawdzamy, czy aktualny tileset zawiera "SpecialBlocks.json"
        if (tileset["source"].get<std::string>().find("SpecialBlocks.json") != std::string::npos)
            specialBlocksFirstGID = tileset["firstgid"];
            // Znajdujemy pierwszy identyfikator GID nast�pnego tilesetu
        else if (tileset["firstgid"] > specialBlocksFirstGID && specialBlocksFirstGID != -1) {
            nextTilesetFirstGID = tileset["firstgid"];
            break;
        }

    if (specialBlocksFirstGID == -1) return result;

    for (const auto &layer: json["layers"]) {
        if (layer["type"] != "tilelayer") continue;
        static constexpr std::uint32_t mask = 0xf0000000;

        int x = 0;
        int y = 0;

        for (uint32_t i: processDataString(layer["data"], width * height, 0)) {
            uint32_t flipFlags = (i & mask) >> 28;
            uint32_t tileID = i & ~mask;

            if (tileID >= specialBlocksFirstGID && tileID < nextTilesetFirstGID) {
                int localTileID = tileID - specialBlocksFirstGID;
                if (localTileID >= 0) result.emplace(glm::ivec2{x, y}, localTileID);
            }
            ++x;
            if (x >= width) {
                x = 0;
                ++y;
            }
        }
    }
    return result;
}

inline sf::Color hexStringToSfmlColor(const std::string &hexColor) {
    const std::string &hex = hexColor[0] == '#' ? hexColor.substr(1) : hexColor;

    std::istringstream iss(hex);
    int rgbValue = 0;
    iss >> std::hex >> rgbValue;

    const uint8 red = rgbValue >> 16 & 0xFF;
    const uint8 green = rgbValue >> 8 & 0xFF;
    const uint8 blue = rgbValue & 0xFF;

    return {red, green, blue};
}

template<typename T>
static T roundTo(T value, int places) {
    T factor = std::pow(10.0, places);
    return std::round(value * factor) / factor;
}

static GameType::MyVec2 roundTo(const GameType::MyVec2 &vec, int places) {
    auto factor = static_cast<float>(std::pow(10.0f, places));
    return GameType::MyVec2{std::round(vec.x * factor) / factor, std::round(vec.y * factor) / factor};
}


template<typename T>
static T convertMetersToPixel(const T meterValue) {
    T result = meterValue * config::meterToPixelRatio;
    return roundTo(result, 2);
}

template<typename T>
static T convertPixelsToMeters(const T pixelValue) {
    T result = pixelValue * config::pixelToMeterRatio;
    return roundTo(result, 10);
}

static sf::Vector2f convertPixelsToMeters(const sf::Vector2f &pixelValue) {
    sf::Vector2f result = {};
    result.x = static_cast<float>(pixelValue.x * config::pixelToMeterRatio);
    result.y = static_cast<float>(pixelValue.y * config::pixelToMeterRatio);

    result.x = roundTo(result.x, 10);
    result.y = roundTo(result.y, 10);

    return result;
}

template<typename T>
T getRandomElement(const std::vector<T> &vec) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, vec.size() - 1);

    return vec[dis(gen)];
}
