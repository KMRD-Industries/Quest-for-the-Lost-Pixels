#pragma once

#include "RenderComponent.h"
#include "System.h"
#include "WeaponComponent.h"

class ItemSystem : public System
{
public:
    void init();
    void update();
    void markClosest();
    void displayWeaponStats(Entity entity);
    void displayHelmetStats(Entity entity);
    void input(Entity player);
    void deleteItems() const;

private:
};
