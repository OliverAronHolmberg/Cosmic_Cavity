#pragma once
#include <vector>
#include <raylib.h>
#include "Items.h"
#include "TextureHandler.h"



class InventorySlot{
    private:
    Item*heldItem = nullptr;
    bool isOccupied = false;
    int x;
    int y;
    int w;
    int h;
    std::string bgTextureID;

    public:
    InventorySlot(int X, int Y, int W, int H);
    ~InventorySlot();

    Rectangle getRec() const {return {(float)x, (float)y, (float)w, (float)h};}
    
    void SetItem(Item* newItem);
    bool getOccupied() const {return isOccupied;}
    std::string getItemName() const;
    std::string getItemID() const;

    void incrementItemCount(int amount);
    Item* getHeldItem() const {return heldItem;}

    void DrawSlot();

};

class Inventory {
    private:
    std::vector<InventorySlot> slots;
    std::vector<InventorySlot> craftingslots;
    int rows, cols;
    int craftingRows, craftingCols;
    int x, y, w, h;
    int selectedSlot;


    public:
    bool isOpened = false;
    bool craftingMenu = false;

    Inventory(int X, int Y, int W, int H);

    void ToggleInventory() {isOpened = !isOpened;}
    void ToggleCraftingMenu() {craftingMenu = !craftingMenu;}
    void setSelectedSlot(int slotNumber) {selectedSlot = slotNumber - 1;}

    std::string getItemID();
    std::string getSelectedItemName();
    Item* getCurrentSelectedItem();

    void DrawInventory(int screenW, int screenH);
    void removeItemFromSelected(int amount);
    bool AddItem(Item* newItem);

    private:
    void highlightSelectedSlot();
};
