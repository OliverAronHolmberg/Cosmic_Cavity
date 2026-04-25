#pragma once
#include <raylib.h>
#include <string>
#include "TextureHandler.h"


class Inventory;
class Item;

class TextureBlock{
    protected:
    int x;
    int y;
    int w;
    int h;
    Rectangle rec;

    public:
    std::string ID;
    Rectangle getRec() const {return rec;}
    Vector2 getPos() const {return {(float)x, (float)y};}

    TextureBlock(int posX, int posY, int W, int H, std::string textureID){
        x = posX;
        y = posY;
        w = W;
        h = H;
        ID = textureID;
    }
    virtual void DrawTile(){
        // Use the tile's actual ID to fetch the correct texture
        DrawTextureScaled(textureAssets.Get(ID), rec);
    }
    virtual ~TextureBlock() = default;
};

class Tile : public TextureBlock{
    public:
    std::string dropID;
    int dropAmount;
    std::string tileName;
    bool isBlockItem;
    bool hasCollision;

    Tile(int posX, int posY, int W, int H, std::string textureID, std::string Name, std::string drop, int amount = 1, bool isBlock=true, bool solid = true)
    : TextureBlock(posX, posY, W, H, textureID){
        tileName = Name;
        dropAmount = amount;
        dropID = drop;
        isBlockItem = isBlock;
        hasCollision = solid;

    }

    Item* CreateDrop();
    virtual void OnInteract(Inventory& playerInv){}
};

class InteractableTile : public Tile{
    public:
    InteractableTile(int posX, int posY, int W, int H, std::string tex, std::string drop)
    : Tile(posX, posY, W, H, tex, tex, drop, 1, true, true){}
};

