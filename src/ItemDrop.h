#pragma once
#include "Entity.h"
#include "Items.h"

// Forward declaration of World so we don't include World.h here
class World;

class ItemDrop : public Entity {
public:
    Item* itemData;
    float age = 0.0f;
    const float maxAge = 120.0f;

    ItemDrop(Item* item, float x, float y, int velX, int velY);
    virtual ~ItemDrop(); 
    void Update(World& world, int tileSize, float dt);
    void Draw();
    bool isExpired() const {return age >= maxAge;}
    void SetVelocity(Vector2 velInput){velocity.x = velInput.x; velocity.y = velInput.y;}
};