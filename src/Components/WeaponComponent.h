#pragma once

struct WeaponComponent
{
    int id{};
    int damageAmount{};
    bool isAttacking{};
    bool queuedAttack{false};
    bool queuedAttackFlag{false};

    bool isSwingingForward{true};
    bool isFacingRight{true};

    float currentAngle{0.f};
    float initialAngle{30.f};
    float rotationSpeed{15.f};

    float swingDistance{90.f};
    float remainingDistance{0};
    float recoilAmount{10.f};

    sf::Vector2f pivotPoint{};
    sf::Vector2f targetPoint{};
    float targetAngleDegrees{};
};
