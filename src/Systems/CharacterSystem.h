#pragma once
#include "System.h"

class CharacterSystem : public System
{
public:
    explicit CharacterSystem() = default;
    void update(const float&);

private:
    void cleanUpDeadEntities() const;
    void performFixedUpdate();
    float m_frameTime{};
};