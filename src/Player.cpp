#include "Player.h"
#include <cmath>


Player::Player(int winW, int winH) : inventory((winW-540) /2, winH-85, 540, 75){

    playerRec = {0, 0, 75, 175};

    camera = { 0 };
    camera.target = {playerRec.x, playerRec.y};
    camera.offset = {winW/2.0f, winH/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    movementSpeed = 8.0f;
};


void Player::Update(World& world, int tileSize) {
    mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
    snappedX = (int)(std::floor(mouseWorldPos.x / tileSize)) * tileSize;
    snappedY = (int)(std::floor(mouseWorldPos.y / tileSize)) * tileSize;

    if (IsKeyPressed(KEY_TAB)) inventory.ToggleInventory();

    for (int i = 0; i < 9; i++) {
        if (IsKeyPressed(KEY_ONE + i)) inventory.setSelectedSlot(i + 1);
    }


    float deltaX = 0;
    if (!inventory.isOpened) {
        if (IsKeyDown(KEY_D)) deltaX += movementSpeed;
        if (IsKeyDown(KEY_A)) deltaX -= movementSpeed;
    }
    playerRec.x += deltaX;


    auto pCoords = world.GetChunkCoords((int)playerRec.x, (int)playerRec.y, tileSize);
    for (int cx = -1; cx <= 1; cx++) {
        for (int cy = -1; cy <= 1; cy++) {
            std::pair<int, int> targetChunk = { pCoords.first + cx, pCoords.second + cy };
            if (world.chunks.count(targetChunk)) {
                for (auto const& tile : world.chunks[targetChunk].tiles) {
                    if (tile.HasCollision() && CheckCollisionRecs(playerRec, tile.getRec())) {
                        if (deltaX > 0) playerRec.x = tile.getRec().x - playerRec.width;
                        if (deltaX < 0) playerRec.x = tile.getRec().x + tile.getRec().width;
                    }
                }
            }
        }
    }


    if (!isFlying) {
        accelerationY += gravity;
    } else {
        if (IsKeyDown(KEY_W)) playerRec.y -= movementSpeed;
        if (IsKeyDown(KEY_S)) playerRec.y += movementSpeed;
        accelerationY = 0;
    }

    playerRec.y += accelerationY;
    isGrounded = false;

    for (int cx = -1; cx <= 1; cx++) {
        for (int cy = -1; cy <= 1; cy++) {
            std::pair<int, int> targetChunk = { pCoords.first + cx, pCoords.second + cy };
            if (world.chunks.count(targetChunk)) {
                for (auto const& tile : world.chunks[targetChunk].tiles) {
                    if (tile.HasCollision() && CheckCollisionRecs(playerRec, tile.getRec())) {
                        if (accelerationY > 0) {
                            playerRec.y = tile.getRec().y - playerRec.height;
                            accelerationY = 0;
                            isGrounded = true;
                        } else if (accelerationY < 0) {
                            playerRec.y = tile.getRec().y + tile.getRec().height;
                            accelerationY = 0;
                        }
                    }
                }
            }
        }
    }

    if (isGrounded && IsKeyDown(KEY_SPACE)) accelerationY = -jumpHeight;

    camera.target = { playerRec.x + playerRec.width / 2, playerRec.y + playerRec.height / 2 };

    if (!inventory.isOpened) {
      
        auto mouseCoords = world.GetChunkCoords(snappedX, snappedY, tileSize);

   
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            bool occupied = false;
            if (world.chunks.count(mouseCoords)) {
                for (auto const& t : world.chunks[mouseCoords].tiles) {
                    if ((int)t.getPos().x == snappedX && (int)t.getPos().y == snappedY) {
                        occupied = true;
                        break;
                    }
                }
            }

            Item* current = inventory.getCurrentSelectedItem();
            if (current && current->count > 0) {
                BlockItem* bItem = dynamic_cast<BlockItem*>(current);
                if (bItem) {
                    bool canPlace = !occupied;
                    Rectangle placementRec = { (float)snappedX, (float)snappedY, (float)tileSize, (float)tileSize };
                    if (bItem->blueprint.hasCollision && CheckCollisionRecs(playerRec, placementRec)) canPlace = false;

                    if (canPlace) {
                        world.AddTile(bItem->blueprint, snappedX, snappedY, tileSize);
                        bItem->count--;
                        inventory.updateActiveSlot();
                    }
                }
            }
        }


        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            if (world.chunks.count(mouseCoords)) {
                auto& chunkTiles = world.chunks[mouseCoords].tiles;
                for (int i = 0; i < (int)chunkTiles.size(); i++) {
                    if (CheckCollisionPointRec(mouseWorldPos, chunkTiles[i].getRec())) {
                        Item* drop = chunkTiles[i].CreateDrop();
                        if (inventory.AddItem(drop)) {
                            // Important: We call our new coordinate-based remove
                            world.RemoveTileAt(snappedX, snappedY, tileSize);
                        } else {
                            delete drop;
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


void Player::Draw(){
    Texture2D sprite = textureAssets.Get("PLAYER");
    float drawW = playerRec.width + 30;
    float drawH = playerRec.height + 25;
    Rectangle dest = {
        playerRec.x - 15, 
        playerRec.y - 25,
        drawW, 
        drawH
    };
    DrawTextureScaled(sprite, dest);
}

void Player::DrawHighlights(int tileSize){
    if(!inventory.isOpened){
        DrawRectangleLinesEx({(float)snappedX, (float)snappedY, (float)tileSize, (float)tileSize}, 3.0f, WHITE);
    }
}
