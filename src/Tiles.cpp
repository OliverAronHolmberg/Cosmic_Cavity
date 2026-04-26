#include "Tiles.h"
#include "Items.h"
#include "Inventory.h"




Item* Tile::CreateDrop() {
    TileDef dropBlueprint = type; 
    
    if (type.dropID != type.name) {
        dropBlueprint.name = type.dropID;
        dropBlueprint.textureID = type.dropID;
    }

    return new BlockItem(dropBlueprint.name, dropBlueprint, type.dropAmount);
}

void Tile::OnInteract(Inventory& playerInv) {

}

