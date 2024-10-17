#pragma once
#include "PublicConfigMenager.h"

extern PublicConfigSingleton configSingleton;

struct WeaponComponent
{
    uint32_t id{};
    int damageAmount{configSingleton.GetConfig().weaponComponentDefaultDamageAmount};
    bool isAttacking{configSingleton.GetConfig().weaponComponentDefaultIsAttacking};
    bool queuedAttack{configSingleton.GetConfig().weaponComponentDefaultQueuedAttack};
    bool queuedAttackFlag{configSingleton.GetConfig().weaponComponentDefaultQueuedAttackFlag};

    bool isSwingingForward{configSingleton.GetConfig().weaponComponentDefaultIsSwingingForward};
    bool isFacingRight{configSingleton.GetConfig().weaponComponentDefaultIsFacingRight};

    float currentAngle{configSingleton.GetConfig().weaponComponentDefaultCurrentAngle};
    float initialAngle{configSingleton.GetConfig().weaponComponentDefaultInitialAngle};
    float rotationSpeed{configSingleton.GetConfig().weaponComponentDefaultRotationSpeed};

    float swingDistance{configSingleton.GetConfig().weaponComponentDefaultSwingDistance};
    float remainingDistance{configSingleton.GetConfig().weaponComponentDefaultRemainingDistance};
    float recoilAmount{configSingleton.GetConfig().weaponComponentDefaultRecoilAmount};

    sf::Vector2f pivotPoint{};
    sf::Vector2f targetPoint{};
    float targetAngleDegrees{};
    bool equipped{false};
    GameType::WeaponType type{GameType::WeaponType::MELEE};
};
