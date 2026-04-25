#pragma once
#include <raylib.h>
#include <string>
#include "TextureHandler.h"


class Inventory;
class Item;

class TextureBlock {
protected:
    int x, y, w, h;
    Rectangle rec;
    std::string ID;

public:
    TextureBlock(int posX, int posY, int W, int H, std::string textureID);
    virtual void DrawTile(); 
    virtual ~TextureBlock() = default;
    Rectangle getRec() const { return rec; }
    Vector2 getPos() const { return {(float)x, (float)y}; }
};

class Tile : public TextureBlock {
public:
    std::string dropID;
    int dropAmount;
    std::string tileName;
    bool isBlockItem;
    bool hasCollision;

    Tile(int posX, int posY, int W, int H, std::string textureID, std::string Name, std::string drop, int amount = 1, bool isBlock = true, bool solid = true);
    void DrawTile() override; 
    Rectangle getRec() const { return TextureBlock::getRec(); }
    Item* CreateDrop();
    virtual void OnInteract(Inventory& playerInv) {}
};



class InteractableTile : public Tile{
    public:
    InteractableTile(int posX, int posY, int W, int H, std::string tex, std::string drop)
    : Tile(posX, posY, W, H, tex, tex, drop, 1, true, true){}
};

