#include "Tiles.h"
#include "Items.h"
#include "Inventory.h"




Item* Tile::CreateDrop() {

    return new BlockItem(type.name, type, type.dropAmount);
}

void Tile::OnInteract(Inventory& playerInv) {

}

