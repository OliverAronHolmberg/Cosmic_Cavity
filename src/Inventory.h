#pragma once
#include <vector>
#include <string>
#include <raylib.h>
#include "Items.h"
#include "TextureHandler.h"

class InventorySlot;


class Container {
public:
    virtual ~Container() {}
    virtual std::vector<InventorySlot>& GetSlots() = 0;
};

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

    void SetHeldItemRaw(Item* item) { 
        heldItem = item; 
        isOccupied = (heldItem != nullptr); 
    }

    void SwapItem(Item*& mouseItem) {
        Item* temp = heldItem;
        heldItem = mouseItem;
        mouseItem = temp;
        isOccupied = (heldItem != nullptr);
    }
    
    void UpdateSlotState();
    void DrawSlot();
};

class Inventory : public Container {
private:
    std::vector<InventorySlot> slots;
    std::vector<InventorySlot> craftingslots;
    int rows, cols;
    int x, y, w, h;
    int selectedSlot;
    Item* mouseItem = nullptr;

public:
    bool isOpened = false;

    Inventory(int X, int Y, int W, int H);
    ~Inventory();

    void ToggleInventory() { isOpened = !isOpened; }
    void setSelectedSlot(int slotNumber);
    
    Item* getCurrentSelectedItem();
    void updateActiveSlot(); 

    std::vector<InventorySlot>& GetSlots() override { return slots; }
    
    void UpdateMouseLogic(Container* activeContainer);
    void DrawMouseItem();
    

    bool AddItem(Item* newItem);
    void DrawInventory(int screenW, int screenH);

private:
    void highlightSelectedSlot();
};