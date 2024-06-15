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

    std::function<bool(unsigned char)> is_base64 = [](unsigned char c) -> bool
    { return (isalnum(c) || (c == '+') || (c == '/')); };

    auto in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
    {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4)
        {
            for (i = 0; i < 4; i++) char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++) ret += char_array_3[i];

            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 4; j++) char_array_4[j] = 0;


        for (j = 0; j < 4; j++) char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
        {
            ret += char_array_3[j];
        }
    }

    return ret;
}

static inline std::vector<uint32_t> processDataString(std::string dataString, std::size_t tileCount,
                                                      int32_t compressionType)
{
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
    for (auto i = 0u; i < expectedSize - 3u; i += 4u)
    {
        uint32_t id = byteData[i] | byteData[i + 1] << 8 | byteData[i + 2] << 16 | byteData[i + 3] << 24;
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

struct IVec2Compare
{
    bool operator()(const glm::ivec2& lhs, const glm::ivec2& rhs) const
    {
        return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
    }
};

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
    return roundTo(result, 5);
}
