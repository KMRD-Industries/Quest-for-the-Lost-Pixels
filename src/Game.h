#pragma once
#include <RenderSystem.h>


#include "Dungeon.h"
#include "FightSystem.h"
#include "Game.h"
#include "PlayerMovementSystem.h"

#include <TextureSystem.h>

#include "AnimationSystem.h"
#include "CharacterSystem.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "EnemySystem.h"
#include "ObjectCreatorSystem.h"
#include "PassageSystem.h"
#include "SpawnerSystem.h"
#include "TextTagSystem.h"
#include "TravellingSystem.h"
#include "WeaponsSystem.h"

class Game {
public:
    void init();

    void draw();

    void update(const float &deltaTime);

    void handleCollision(const float &deltaTime) const;

    sf::Color getBackground();

private:
    Dungeon m_dungeon;

    CollisionSystem *m_collisionSystem;
    RenderSystem *m_renderSystem;
    ObjectCreatorSystem *m_objectCreatorSystem;
    FightSystem *m_fightSystem;
};
