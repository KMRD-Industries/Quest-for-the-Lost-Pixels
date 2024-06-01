#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <cstdint>
#include <functional>
#include <glm/glm.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <unordered_map>

#include "Config.h"
#include "glm/gtx/hash.hpp"

static inline std::string base64_decode(const std::string& encoded_string)
{
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    auto is_base64 = [](unsigned char c) -> bool { return ((std::isalnum(c) != 0) || (c == '+') || (c == '/')); };

    std::string ret;
    std::array<unsigned char, 4> char_array_4{};
    std::array<unsigned char, 3> char_array_3{};
    size_t in_len = encoded_string.size();
    size_t in_ = 0;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
    {
        char_array_4[0] = encoded_string[in_++];
        char_array_4[1] = encoded_string[in_++];
        char_array_4[2] = encoded_string[in_++];
        char_array_4[3] = encoded_string[in_++];

        for (int i = 0; i < 4; ++i) char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (int i = 0; i < 3; ++i) ret += char_array_3[i];
    }

    return ret;
}


static inline std::vector<uint32_t> processDataString(std::string dataString, std::size_t tileCount,
                                                      int32_t compressionType)
{
    std::string decodedData = base64_decode(dataString);

    std::vector<unsigned char> byteData(decodedData.begin(), decodedData.end());

    std::vector<uint32_t> IDs;
    IDs.reserve(tileCount);

    for (size_t i = 0; i < byteData.size() - 3; i += 4)
    {
        uint32_t id = byteData[i] | (byteData[i + 1] << 8) | (byteData[i + 2] << 16) | (byteData[i + 3] << 24);
        IDs.push_back(id);
    }

    return IDs;
};

static std::string extractFileName(const std::string& filePath, const std::string& begin, const std::string& end)
{
    // Find the position of the last '/'
    size_t lastSlashPos = filePath.find_last_of(begin);
    if (lastSlashPos == std::string::npos)
    {
        // If '/' is not found, return an empty string or handle error accordingly
        return "";
    }

    // Find the position of ".tsx"
    size_t tsxPos = filePath.find(end, lastSlashPos);
    if (tsxPos == std::string::npos)
    {
        // If ".tsx" is not found, return an empty string or handle error accordingly
        return "";
    }

    // Extract the substring between lastSlashPos and tsxPos
    return filePath.substr(lastSlashPos + 1, tsxPos - lastSlashPos - 1);
}


static std::unordered_multimap<glm::ivec2, int> findSpecialBlocks(const nlohmann::json& json)
{
    std::unordered_multimap<glm::ivec2, int> result;

    const int width = json["width"];
    const int height = json["height"];

    int specialBlocksFirstGID = -1;
    int nextTilesetFirstGID = INT_MAX; // Pocz�tkowy identyfikator GID nast�pnego tilesetu

    for (const auto& tileset : json["tilesets"])
    {
        // Sprawdzamy, czy aktualny tileset zawiera "SpecialBlocks.json"
        if (tileset["source"].get<std::string>().find("SpecialBlocks.json") != std::string::npos)
        {
            specialBlocksFirstGID = tileset["firstgid"];
        }
        // Znajdujemy pierwszy identyfikator GID nast�pnego tilesetu
        else if (tileset["firstgid"] > specialBlocksFirstGID && specialBlocksFirstGID != -1)
        {
            nextTilesetFirstGID = tileset["firstgid"];
            break;
        }
    }

    if (specialBlocksFirstGID == -1)
    {
        return result;
    }

    for (const auto& layer : json["layers"])
    {
        if (layer["type"] != "tilelayer")
        {
            continue;
        }
        static constexpr std::uint32_t mask = 0xf0000000;

        int x = 0;
        int y = 0;

        for (uint32_t i : processDataString(layer["data"], width * height, 0))
        {
            uint32_t flipFlags = (i & mask) >> 28;
            uint32_t tileID = i & ~mask;

            if (tileID >= specialBlocksFirstGID && tileID < nextTilesetFirstGID)
            {
                int localTileID = tileID - specialBlocksFirstGID;
                if (localTileID >= 0)
                {
                    result.emplace(glm::ivec2{x, y}, localTileID);
                }
            }
            ++x;
            if (x >= width)
            {
                x = 0;
                ++y;
            }
        }
    }
    return result;
}

template <typename T>
inline static T roundTo(T value, int places)
{
    T factor = std::pow(10.0, places);
    return std::round(value * factor) / factor;
}

template <typename T>
inline static T convertMetersToPixel(const T meterValue)
{
    T result = meterValue * config::meterToPixelRatio;
    return roundTo(result, 2);
}

template <typename T>
inline static T convertPixelsToMeters(const T pixelValue)
{
    T result = pixelValue * config::pixelToMeterRatio;
    return roundTo(result, 2);
}
