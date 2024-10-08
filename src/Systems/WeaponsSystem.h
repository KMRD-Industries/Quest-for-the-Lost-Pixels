#pragma once

#include "System.h"

class WeaponSystem : public System {
public:
    void update(const float &deltaTime);

    void performFixedUpdate();

    void deleteItems() const;

private:
    void updateWeaponAngle(Entity);

    void updateStartingAngle(Entity);

    void setAngle(Entity);

    void rotateWeapon(Entity, bool);

    float m_frameTime{};
};
