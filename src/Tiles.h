#pragma once
#include <raylib.h>
#include <string>
#include "TextureHandler.h"


class Inventory;
class Item;

enum class TileShape {
    FULL_BLOCK,
    SLAB_BOTTOM,
    SLAB_TOP,
    STAIRS_RIGHT,
    STAIRS_LEFT,
    WALL
};

struct TileDef {
    std::string name;
    std::string textureID;
    std::string dropID;
    int dropAmount = 1;
    bool hasCollision = true;
    TileShape shape = TileShape::FULL_BLOCK;
};

class Tile {
private:
    Vector2 pos;
    float width, height;
    TileDef type;

public:
    Tile(float x, float y, float w, float h, TileDef definition) 
        : pos({x, y}), width(w), height(h), type(definition) {}


    Rectangle getRec() const {
        if (type.shape == TileShape::SLAB_BOTTOM) {
            return { pos.x, pos.y + (height / 2), width, height / 2 };
        }
        if (type.shape == TileShape::SLAB_TOP) {
            return { pos.x, pos.y, width, height / 2 };
        }

        return { pos.x, pos.y, width, height }; 
    }

    void Draw() {
        Rectangle dest = { pos.x, pos.y, width, height };
        

        if (type.shape == TileShape::SLAB_BOTTOM || type.shape == TileShape::SLAB_TOP) {
            dest.height /= 2;
            if (type.shape == TileShape::SLAB_BOTTOM) dest.y += height / 2;
        }
        
        DrawTextureScaled(textureAssets.Get(type.textureID), dest);
    }

 
    std::string getDropID() const { return type.dropID; }
    int getDropAmount() const { return type.dropAmount; }
    Vector2 getPos() const { return pos; }
    bool HasCollision() const { return type.hasCollision && type.shape != TileShape::WALL; }
    std::string GetName() const { return type.name; }

    Item* CreateDrop();
    void OnInteract(Inventory& playerInv);
};