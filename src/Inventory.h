#pragma once
#include <vector>
#include <string>
#include <raylib.h>
#include "Items.h"
#include "TextureHandler.h"

class InventorySlot {
private:
    Item* heldItem = nullptr;
    bool isOccupied = false;
    int x, y, w, h;
    std::string bgTextureID;

public:
    InventorySlot(int X, int Y, int W, int H);
    ~InventorySlot();

    
    InventorySlot(const InventorySlot&) = delete;
    InventorySlot& operator=(const InventorySlot&) = delete;


    InventorySlot(InventorySlot&& other) noexcept;
    InventorySlot& operator=(InventorySlot&& other) noexcept;

    Rectangle getRec() const { return {(float)x, (float)y, (float)w, (float)h}; }
    
    void SetItem(Item* newItem);
    Item* getHeldItem() const { return heldItem; }
    bool getOccupied() const { return isOccupied; }
    
    void UpdateSlotState();
    void DrawSlot();
};

class Inventory {
private:
    std::vector<InventorySlot> slots;
    std::vector<InventorySlot> craftingslots;
    int rows, cols;
    int x, y, w, h;
    int selectedSlot;

public:
    bool isOpened = false;
    bool craftingMenu = false;

    Inventory(int X, int Y, int W, int H);

    void ToggleInventory() { isOpened = !isOpened; }
    void setSelectedSlot(int slotNumber);
    
    Item* getCurrentSelectedItem();
    void updateActiveSlot(); 
    
    bool AddItem(Item* newItem);
    void DrawInventory(int screenW, int screenH);

private:
    void highlightSelectedSlot();
};