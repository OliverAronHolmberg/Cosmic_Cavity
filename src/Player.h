#pragma once
#include <raylib.h>
#include <vector>
#include "Inventory.h"
#include "World.h"


class Player {
    private:

    float movementSpeed;
    float gravity = 0.5f;
    float jumpHeight = 20.0f;
    float accelerationY = 0.0f;
    bool isGrounded = false;
    bool isFlying = false;

    Camera2D camera;
    Inventory inventory;

    int snappedX, snappedY;
    Vector2 mouseWorldPos;

    public:
    Rectangle playerRec;
    Player(int winW, int winH);

    void Update(World& world, int tileSize);

    void Draw();
    void DrawHighlights(int tileSize);

    Inventory& getInventory() {return inventory;}
    Camera2D getCamera() {return camera;}
    Vector2 GetMouseWorldPos() {return mouseWorldPos;}
};