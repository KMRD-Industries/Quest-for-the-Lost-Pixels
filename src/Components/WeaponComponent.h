#pragma once
#include "Config.h"

struct WeaponComponent
{
    int id{};
    int damageAmount{config::weaponComponentDefaultDamageAmount};
    bool isAttacking{config::weaponComponentDefaultIsAttacking};
    bool queuedAttack{config::weaponComponentDefaultQueuedAttack};
    bool queuedAttackFlag{config::weaponComponentDefaultQueuedAttackFlag};

    bool isSwingingForward{config::weaponComponentDefaultIsSwingingForward};
    bool isFacingRight{config::weaponComponentDefaultIsFacingRight};

    float currentAngle{config::weaponComponentDefaultCurrentAngle};
    float initialAngle{config::weaponComponentDefaultInitialAngle};
    float rotationSpeed{config::weaponComponentDefaultRotationSpeed};

    float swingDistance{config::weaponComponentDefaultSwingDistance};
    float remainingDistance{config::weaponComponentDefaultRemainingDistance};
    float recoilAmount{config::weaponComponentDefaultRecoilAmount};

    sf::Vector2f pivotPoint{};
    sf::Vector2f targetPoint{};
    float targetAngleDegrees{};
    bool equipped{false};
    GameType::WeaponType type{GameType::WeaponType::UNKNOWN};

};
