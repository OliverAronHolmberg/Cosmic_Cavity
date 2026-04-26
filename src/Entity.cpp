#include "Entity.h"


Entity::Entity(float x, float y, float w, float h, float speed) {
    rect = { x, y, w, h };
    velocity = { 0, 0 };
    movementSpeed = speed;
}

void Entity::ApplyPhysics(World& world, int tileSize, float dt) {

    rect.x += velocity.x * dt * 60.0f;


    auto pCoords = world.GetChunkCoords((int)rect.x, (int)rect.y, tileSize);
    for (int cx = -1; cx <= 1; cx++) {
        for (int cy = -1; cy <= 1; cy++) {
            std::pair<int, int> target = { pCoords.first + cx, pCoords.second + cy };
            if (world.chunks.count(target)) {
                for (const auto& tile : world.chunks[target].tiles) {
                    if (tile.HasCollision() && CheckCollisionRecs(rect, tile.getRec())) {
                        if (velocity.x > 0) rect.x = tile.getRec().x - rect.width;
                        if (velocity.x < 0) rect.x = tile.getRec().x + tile.getRec().width;
                    }
                }
            }
        }
    }


    if (!isFlying) {
        velocity.y += gravity;
    }
    
    rect.y += velocity.y * 60.0f * dt;
    isGrounded = false;


    for (int cx = -1; cx <= 1; cx++) {
        for (int cy = -1; cy <= 1; cy++) {
            std::pair<int, int> target = { pCoords.first + cx, pCoords.second + cy };
            if (world.chunks.count(target)) {
                for (const auto& tile : world.chunks[target].tiles) {
                    if (tile.HasCollision() && CheckCollisionRecs(rect, tile.getRec())) {
                        if (velocity.y > 0) {
                            rect.y = tile.getRec().y - rect.height;
                            velocity.y = 0;
                            isGrounded = true;
                        } 
                        else if (velocity.y < 0) { 
                            rect.y = tile.getRec().y + tile.getRec().height;
                            velocity.y = 0;
                        }
                    }
                }
            }
        }
    }
}