#pragma once

struct WeaponComponent
{
    int weaponID{};
    int damage{};
    bool isAttacking{};
    bool swingingForward{true};
    bool isFacingLeftToRight{true};
    float angle{0.f};
    float startingAngle{30.f};
    float rotationSpeed{15.f};
    float maxAngle{120.f};
    float recoil{10.f};
    sf::Vector2i pivot{};
    sf::Vector2i atan{};
};
