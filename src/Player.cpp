#include "Player.h"
#include "TextureHandler.h"
#include <cmath>

Player::Player(int winW, int winH) 
    : Entity(100.0f, 100.0f, 75.0f, 175.0f, 8.0f), 
      inventory((winW - 540) / 2, winH - 85, 540, 75) 
{
    camera = { 0 };
    camera.offset = { winW / 2.0f, winH / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void Player::Update(World& world, int tileSize) {
    float dt = GetFrameTime();
    mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
    snappedX = (int)(std::floor(mouseWorldPos.x / tileSize)) * tileSize;
    snappedY = (int)(std::floor(mouseWorldPos.y / tileSize)) * tileSize;

    if (IsKeyPressed(KEY_TAB)) inventory.ToggleInventory();

    for (int i = 0; i < 9; i++) {
        if (IsKeyPressed(KEY_ONE + i)) inventory.setSelectedSlot(i + 1);
    }


    velocity.x = 0; 
    if (!inventory.isOpened) {
        if (IsKeyDown(KEY_D)) velocity.x = movementSpeed;
        if (IsKeyDown(KEY_A)) velocity.x = -movementSpeed;
        
        if (isGrounded && IsKeyDown(KEY_SPACE)) {
            velocity.y = -jumpHeight;
        }

        if (isFlying) {
            velocity.y = 0;
            if (IsKeyDown(KEY_W)) velocity.y = -movementSpeed;
            if (IsKeyDown(KEY_S)) velocity.y = movementSpeed;
        }
    }


    ApplyPhysics(world, tileSize, dt);

   
    camera.target = { rect.x + rect.width / 2.0f, rect.y + rect.height / 2.0f };


    if (!inventory.isOpened) {
        auto mouseCoords = world.GetChunkCoords(snappedX, snappedY, tileSize);

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    Item* current = inventory.getCurrentSelectedItem();
    if (current && current->count > 0) {
        BlockItem* bItem = dynamic_cast<BlockItem*>(current);
        if (bItem) {
            auto mouseCoords = world.GetChunkCoords(snappedX, snappedY, tileSize);
            bool occupied = false;

            if (world.chunks.count(mouseCoords)) {
                for (const auto& t : world.chunks[mouseCoords].tiles) {
              
                    if ((int)t.getPos().x == snappedX && (int)t.getPos().y == snappedY) {
                        occupied = true;
                        break;
                    }
                }
            }

          
            Rectangle placementRec = { (float)snappedX, (float)snappedY, (float)tileSize, (float)tileSize };
            
            if (!occupied && !CheckCollisionRecs(rect, placementRec)) {
                world.AddTile(bItem->blueprint, snappedX, snappedY, tileSize);
                bItem->count--;
                inventory.updateActiveSlot();
            }
        }
    }
}

   
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            if (world.chunks.count(mouseCoords)) {
                auto& tiles = world.chunks[mouseCoords].tiles;
                for (size_t i = 0; i < tiles.size(); i++) {
                    if (CheckCollisionPointRec(mouseWorldPos, tiles[i].getRec())) {
                        Item* dropData = tiles[i].CreateDrop();
                        
                        if (dropData) {
                            float spawnX = tiles[i].getPos().x + (tileSize / 4.0f);
                            float spawnY = tiles[i].getPos().y + (tileSize / 4.0f);
                         
                            world.SpawnPhysicalDrop(dropData, spawnX, spawnY, 0, 0);
                            
                            world.RemoveTileAt(snappedX, snappedY, tileSize);
                        }
                        break;
                    }
                }
            }
        }


        if (IsKeyPressed(KEY_E)) {
            if (world.chunks.count(mouseCoords)) {
                for (auto& tile : world.chunks[mouseCoords].tiles) {
                    if (CheckCollisionPointRec(mouseWorldPos, tile.getRec())) {
                        tile.OnInteract(inventory, world, tileSize);
                        break;
                    }
                }
            }
        }
    }
}

void Player::Draw() {
    Texture2D sprite = textureAssets.Get("PLAYER");
    Rectangle dest = { rect.x - 15, rect.y - 25, rect.width + 30, rect.height + 25 };
    DrawTextureScaled(sprite, dest);
}

void Player::DrawHighlights(int tileSize) {
    if (!inventory.isOpened) {
        DrawRectangleLinesEx({ (float)snappedX, (float)snappedY, (float)tileSize, (float)tileSize }, 3.0f, WHITE);
    }
}