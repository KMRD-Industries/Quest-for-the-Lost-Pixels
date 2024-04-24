//
// Created by dominiq on 3/28/24.
//

#ifndef TILE_H
#define TILE_H
#include "TextureAtlas.h"

class Tile {
   private:
    unsigned int tileID = 0;
    bool verticalFlip = false;
    bool horizontalFlip = false;
    bool diagonalFlip = false;
    sf::Vector2f position;

   public:
    Tile(const uint32_t id, bool verticalFlip, bool horizontalFlip,
         bool diagonalFlip, sf::Vector2f pos)
        : tileID{id},
          verticalFlip{verticalFlip},
          diagonalFlip{diagonalFlip},
          horizontalFlip{horizontalFlip},
          position(pos){};

    [[maybe_unused]] [[nodiscard]] unsigned int getId() const {
        return tileID;
    };
    [[maybe_unused]] [[nodiscard]] bool isFlippedVertically() const {
        return verticalFlip;
    };
    [[maybe_unused]] [[nodiscard]] bool isFlippedHorizontally() const {
        return horizontalFlip;
    };
    [[maybe_unused]] [[nodiscard]] bool isFlippedDiagonally() const {
        return diagonalFlip;
    };

    [[nodiscard]] float getRotation() const {
        if (verticalFlip)
            return 180.f;  // Adjust rotation for vertical flipping

        return 0.f;
    }

    [[nodiscard]] float getScale() const {
        if (horizontalFlip)
            return -1.f;  // Adjust rotation for vertical flipping

        return 1.f;
    }


    const sf::Vector2f& getPosition() {
        return this->position;
    };

};



#endif //TILE_H
