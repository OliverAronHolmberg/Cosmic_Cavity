#pragma once
#include "Entity.h"
#include "Inventory.h"
#include "World.h"


class Player : public Entity {
private:
    Camera2D camera;
    Inventory inventory;

    int snappedX, snappedY;
    Vector2 mouseWorldPos;

public:
    Player(int winW, int winH);

    void Update(World& world, int tileSize, float dt, bool& debugMode);
    void Draw();
    void DrawHighlights(int tileSize);

    Inventory& getInventory() { return inventory; }
    Camera2D getCamera() { return camera; }
    Vector2 GetMouseWorldPos() { return mouseWorldPos; }
    
    void SetFlying(bool state) { isFlying = state; }


    bool facingRight = true;

    
};