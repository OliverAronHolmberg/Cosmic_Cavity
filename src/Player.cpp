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


void Player::Update(World& world, int tileSize){
    mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
    snappedX = (int)(std::floor(mouseWorldPos.x / tileSize)) * tileSize;
    snappedY = (int)(std::floor(mouseWorldPos.y / tileSize)) * tileSize;

    if(IsKeyPressed(KEY_TAB)) inventory.ToggleInventory();

    for (int i = 0; i < 9; i++){
        if(IsKeyPressed(KEY_ONE + i)) inventory.setSelectedSlot(i+1);
    }

    float deltaX = 0;
    if(!inventory.isOpened){
        if(IsKeyDown(KEY_D)) deltaX += movementSpeed;
        if(IsKeyDown(KEY_A)) deltaX -= movementSpeed;
    }
    playerRec.x += deltaX;

    for (auto const& tile : world.tiles){
        if(tile.HasCollision() && CheckCollisionRecs(playerRec, tile.getRec())){
            if(deltaX > 0) playerRec.x = tile.getRec().x - playerRec.width;
            if(deltaX < 0) playerRec.x = tile.getRec().x + tile.getRec().width;
        }
    }

    if(!isFlying){
        accelerationY += gravity;
    }else{
        if(IsKeyDown(KEY_W)) playerRec.y -= movementSpeed;
        if(IsKeyDown(KEY_S)) playerRec.y += movementSpeed;
        accelerationY = 0;
    }

    playerRec.y += accelerationY;
    isGrounded = false;

    for(auto const& tile : world.tiles){
        if(tile.HasCollision() && CheckCollisionRecs(playerRec, tile.getRec())){
            if(accelerationY > 0){
                playerRec.y = tile.getRec().y - playerRec.height;
                accelerationY = 0;
                isGrounded = true;
            }else if(accelerationY < 0){
                playerRec.y = tile.getRec().y + tile.getRec().height;
                accelerationY = 0;
            }
        }
    }

    if(isGrounded && (IsKeyDown(KEY_SPACE))){
        accelerationY = -jumpHeight;
    }

    camera.target = {playerRec.x + playerRec.width / 2, playerRec.y + playerRec.height/2};


    if(!inventory.isOpened){

        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            bool occupied = false;
            Rectangle placementRec = { (float)snappedX, (float)snappedY, (float)tileSize, (float)tileSize };

            for (auto const& t : world.tiles) {
                if (t.getPos().x == snappedX && t.getPos().y == snappedY) {
                    occupied = true;
                    break;
                }
            }

            Item* current = inventory.getCurrentSelectedItem();
            if (current && current->count > 0) {
                BlockItem* bItem = dynamic_cast<BlockItem*>(current);
                
                if (bItem) {
                    bool canPlace = !occupied;
                    if (bItem->blueprint.hasCollision && CheckCollisionRecs(playerRec, placementRec)) {
                        canPlace = false;
                    }

                    if (canPlace) {
                        world.tiles.push_back(world.CreateTileFromBlueprint(bItem->blueprint, snappedX, snappedY, tileSize));
                        
                        bItem->count--;
                        inventory.updateActiveSlot();
                    }
                }
            }
        }
  
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            for (int i = 0; i < (int)world.tiles.size(); i++) {
                if (CheckCollisionPointRec(mouseWorldPos, world.tiles[i].getRec())) {
                    Item* drop = world.tiles[i].CreateDrop();
      
                    if (inventory.AddItem(drop)) {
                        world.RemoveTileAt(i);
                    } else {
                        delete drop;
                    }
                    
                    break;
                }
            }
        }


        if(IsKeyPressed(KEY_E)){
            for (auto& tile : world.tiles){
                if(CheckCollisionPointRec(mouseWorldPos, tile.getRec())){
                    tile.OnInteract(inventory);
                    break;
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
