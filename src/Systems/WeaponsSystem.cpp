#include "WeaponsSystem.h"

#include "AnimationSystem.h"
#include "WeaponComponent.h"

void WeaponSystem::update() const
{
    for (const auto entity : m_entities)
    {
        auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);

        if (weaponComponent.isAttacking)
        {
            if (weaponComponent.swingingForward)
            {
                weaponComponent.angle += weaponComponent.rotationSpeed;
                if (weaponComponent.angle >= weaponComponent.maxAngle)
                {
                    weaponComponent.angle = weaponComponent.maxAngle;
                    weaponComponent.swingingForward = false;
                }
            }
            else
            {
                weaponComponent.angle -= weaponComponent.rotationSpeed;
                if (weaponComponent.angle <= 30.f)
                {
                    weaponComponent.angle = 30.f;
                    weaponComponent.isAttacking = false;
                    weaponComponent.swingingForward = true;
                }
            }
        }
    }
}
