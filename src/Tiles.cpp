#include "Tiles.h"
#include "Items.h"
#include "Inventory.h"


TextureBlock::TextureBlock(int posX, int posY, int W, int H, std::string textureID) {
    x = posX; y = posY; w = W; h = H;
    ID = textureID;
    rec = {(float)x, (float)y, (float)w, (float)h};
}


void TextureBlock::DrawTile() {
    DrawTextureScaled(textureAssets.Get(ID), rec);
}





Tile::Tile(int posX, int posY, int W, int H, std::string textureID, std::string Name, std::string drop, int amount, bool isBlock, bool solid)
    : TextureBlock(posX, posY, W, H, textureID) 
{
    tileName = Name;
    dropID = drop;
    dropAmount = amount;
    isBlockItem = isBlock;
    hasCollision = solid;
}

void Tile::DrawTile() {
    DrawTextureScaled(textureAssets.Get(ID), rec);
}

Item* Tile::CreateDrop() {
    return new BlockItem(tileName, dropID, dropAmount);
}