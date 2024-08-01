#pragma once

struct WeaponComponent
{
    int weaponID{};
    int damage{};
    bool isAttacking{};
    float angle = 30.f;
    float rotationSpeed = 35.f;
    float maxAngle = 120.f;
    bool swingingForward = true;
};
