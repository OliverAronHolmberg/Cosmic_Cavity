#pragma once
#include <raylib.h>
#include "World.h"

class Entity {
protected:
    Rectangle rect;
    Vector2 velocity;
    float movementSpeed;
    float gravity = 0.5f;
    float jumpHeight = 20.0f;
    bool isGrounded = false;
    bool isFlying = false; 

public:
    Entity(float x, float y, float w, float h, float speed);
    virtual ~Entity() = default;

    void ApplyPhysics(World& world, int tileSize);

    Rectangle GetRect() const { return rect; }
    bool IsGrounded() const { return isGrounded; }
};