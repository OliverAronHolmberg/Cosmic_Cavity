#include "Tiles.h"
#include "Items.h"
#include "Inventory.h"

// The Tile Drop logic
Item* Tile::CreateDrop() {
    // If it's a block, we drop a BlockItem so we can place it again
    return new BlockItem(tileName, dropID, 1);
}

// The Crafter Constructor
Crafter::Crafter(int posX, int posY, float W, float H, bool solid) 
    : InteractableTile(posX, posY, W, H, "CRAFTER", "CRAFTER") {
    hasCollision = solid;
}

// What happens when you press 'E' on a Crafter
void Crafter::OnInteract(Inventory& inv) {
    inv.isOpened = true;
    inv.craftingMenu = true;
}