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

    virtual void OnUse(World& world, int x, int y) {}
};

class BlockItem : public Item {
public:
    TileDef blueprint; 

   
    BlockItem(std::string Name, TileDef tileData, int amount = 1)
        : Item(Name, tileData.textureID, amount, 999, true), blueprint(tileData) {}

    void OnUse(World& world, int x, int y) override;
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
    int resultAmount;
    std::vector<Ingredient> ingredients;
    bool isBlock;
};