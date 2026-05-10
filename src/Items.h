#pragma once
#include <string>
#include <vector>
#include "Tiles.h"

class World;

class Item {
public:
    std::string name;
    std::string textureID;
    int count;
    int max;
    bool isPlaceable; 

    Item(std::string Name, std::string texID, int amount = 1, int maxAmount = 999, bool placeable = false)
        : name(Name), textureID(texID), count(amount), max(maxAmount), isPlaceable(placeable) {}

    virtual ~Item() = default;

    virtual Item* Clone() const { return new Item(name, textureID, count, max, isPlaceable); }

    virtual void OnUse(World& world, int x, int y) {}
};

class BlockItem : public Item {
public:
    TileDef blueprint; 

   
    BlockItem(std::string Name, TileDef tileData, int amount = 1)
        : Item(Name, tileData.textureID, amount, 999, true), blueprint(tileData) {}

    Item* Clone() const override { 
        return new BlockItem(*this); 
    }

    void OnUse(World& world, int x, int y) override;
};

class ExplosiveBlock : public BlockItem{
    public:
    float blastRadius;

    ExplosiveBlock(std::string Name, TileDef tileData, int amount = 1, float radius = 150.0f)
    : BlockItem(Name, tileData, amount), blastRadius(radius) {}

    Item* Clone() const override { 
        return new ExplosiveBlock(*this); 
    }

    void OnUse(World& world, int x, int y, int tileSize);
};


class ToolItem : public Item {
public:
    int power; 
    ToolItem(std::string Name, std::string texID, int toolPower = 1) 
        : Item(Name, texID, 1, 1, false), power(toolPower) {}
    
    void OnUse(World& world, int x, int y) override;
};

struct Ingredient {
    std::string id;
    int amount;
};

struct Recipe { 
    std::string resultID;
    std::string resultTexture;
    int resultAmount;
    std::vector<Ingredient> ingredients;
    bool isBlock;
    TileShape shape;
    std::string grid[9]; // 3x3 grid pattern, empty string means empty slot
};

struct FurnaceRecipe {
    std::string inputID;
    std::string resultID;
    std::string resultTexture;
    int resultAmount;
    int burnTime;
};