#pragma once
#include <string>
#include <vector>


class World;

class Item {
    public:
    std::string name;
    std::string textureID;
    int count;
    int max;

    Item(std::string Name, std::string texID, int amount = 1, int maxAmount = 999)
    : name(Name), textureID(texID), count(amount), max(maxAmount) {}

    virtual ~Item() = default;
    virtual void OnUse() {}
};

class BlockItem : public Item {
    public:
    std::string placeTileID;
    bool isInteractable;
    bool canCollide;

    BlockItem(std::string Name, std::string texID, int amount, bool interactable = false, bool solid = true)
    : Item(Name, texID, amount, 999), placeTileID(texID), isInteractable(interactable), canCollide(solid) {}

    void OnUse() override {}
};

class ToolItem : public Item {
    public:
    ToolItem(std::string Name, std::string texID) : Item(Name, texID, 1, 1) {}
    void OnUse() override {}
};

struct Ingredient {
    std::string id;
    int amount;
};

struct Recipie {
    std::string resultID;
    int resultAmount;
    std::vector<Ingredient> ingredients;
    bool isblock;
};