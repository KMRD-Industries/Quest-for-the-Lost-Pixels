#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <cstdint>
#include <functional>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <unordered_map>
#include "glm/gtx/hash.hpp"

static inline std::string base64_decode(std::string const& encoded_string)
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

    int specialBlocksFirstGID = -1;
    for (const auto& tileset : json["tilesets"])
    {
        if (tileset["source"].get<std::string>().find("SpecialBlocks.json") != std::string::npos)
        {
            specialBlocksFirstGID = tileset["firstgid"];
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
        std::string encoded_data = layer["data"];
        std::string decoded_data = base64_decode(encoded_data);

        std::vector<uint32_t> tileIDs(decoded_data.size() / 4);
        for (size_t i = 0; i < tileIDs.size(); ++i)
        {
            tileIDs[i] = static_cast<uint32_t>(decoded_data[i * 4]) |
                (static_cast<uint32_t>(decoded_data[i * 4 + 1]) << 8) |
                (static_cast<uint32_t>(decoded_data[i * 4 + 2]) << 16) |
                (static_cast<uint32_t>(decoded_data[i * 4 + 3]) << 24);
        }

        int width = layer["width"];
        int height = layer["height"];

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int index = y * width + x;
                uint32_t globalTileID = tileIDs[index];

                if (globalTileID >= specialBlocksFirstGID)
                {
                    int localTileID = globalTileID - specialBlocksFirstGID;
                    if (localTileID >= 0)
                    {
                        result.emplace(glm::ivec2{x, y}, localTileID);
                    }
                }
            }
        }
    }

    return result;
}