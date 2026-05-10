#include "Tiles.h"
#include "Items.h"
#include "Inventory.h"
#include "World.h"




Item* Tile::CreateDrop() {
    TileDef dropBlueprint = type; 
    
    if (type.dropID != type.name) {
        dropBlueprint.name = type.dropID;
        dropBlueprint.textureID = type.dropID;
    }

    return new BlockItem(dropBlueprint.name, dropBlueprint, type.dropAmount);
}

void Tile::OnInteract(Inventory& playerInv, World& world, int tileSize) {
    if (type.name == "CRAFTER") {
        playerInv.craftingMenu = !playerInv.craftingMenu;
        playerInv.isOpened = playerInv.craftingMenu;
    }
    if (type.isExplosive) {
        float centerX = pos.x + (tileSize / 2.0f);
        float centerY = pos.y + (tileSize / 2.0f);
        
        world.Explode({centerX, centerY}, type.explosionRadius, tileSize);
    }
}


void CrafterTile::OnInteract(Inventory& playerInv, World& world, int tileSize) {
    playerInv.isOpened = true;
}