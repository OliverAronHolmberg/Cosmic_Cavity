#include "Inventory.h"

InventorySlot::InventorySlot(int X, int Y, int W, int H){
    x = X;
    y = Y;
    w = W;
    h = H;
    bgTextureID = "INVENTORYSLOT";
}

InventorySlot::~InventorySlot(){
    if(heldItem != nullptr){
        delete heldItem;
    }
}

void InventorySlot::SetItem(Item* newItem){
    heldItem = newItem;
    isOccupied = (heldItem != nullptr);
}

std::string InventorySlot::getItemName() const {
    return isOccupied ? heldItem->name : "NONE";
}

std::string InventorySlot::getItemID() const {
    if (!isOccupied || heldItem->count <= 0) return "NONE";
    return heldItem->textureID;
}

void InventorySlot::incrementItemCount(int amount){
    if(isOccupied && heldItem){
        heldItem->count += amount;
        if(heldItem->count <= 0){
            delete heldItem;
            heldItem = nullptr;
            isOccupied = false;
        }
    }
}

void InventorySlot::DrawSlot() {
    DrawTextureScaled(textureAssets.Get(bgTextureID), getRec());

    if(isOccupied && heldItem){
        Rectangle itemRec = { (float)x+5, (float)y+5, (float)w-10, (float)h-10};
        DrawTextureScaled(textureAssets.Get(heldItem->textureID), itemRec);
        DrawText(TextFormat("%i", heldItem->count), x+5, y+5, 20, WHITE);
    }
}

Inventory::Inventory(int X, int Y, int W, int H){
    x = X; y = Y; w = W; h = H;
    rows = 5; cols = 9;
    craftingRows = 3; craftingCols = 3;
    selectedSlot = 0;

    int slotSize = 75;
    int padding = 0;
    int totalGridWidth = (cols * slotSize) + ((cols-1)*padding);
    int centeredOffset = (W-totalGridWidth) /2;

    for (int r = 0; r < rows; r++){
        for (int c = 0; c < cols; c++){
            int slotX = X + centeredOffset + (c* (slotSize + padding));
            int slotY = Y - (r * (slotSize + padding));
            slots.push_back(InventorySlot(slotX, slotY, slotSize, slotSize));
        }
    }
    
    for (int rc = 0; rc < craftingCols; rc++){
        for(int rr = 0; rr<craftingRows; rr++){
            int slotX = X + centeredOffset + (rc* (slotSize + padding));
            int slotY =(Y-((rows-1)*(slotSize+padding))) -(slotSize+padding) -(rr * (slotSize+padding)) - 20;
            craftingslots.push_back(InventorySlot(slotX, slotY, slotSize, slotSize));
        }
    }
    int resultX = X + centeredOffset + (2* (slotSize + padding)) + 200;
    int resultY = (Y-((rows-1)*(slotSize+padding))) -(slotSize+padding) -(1 * (slotSize+padding)) - 20;
    craftingslots.push_back(InventorySlot(resultX, resultY, slotSize, slotSize));
}

std::string Inventory::getItemID(){ return slots[selectedSlot].getItemID(); }
std::string Inventory::getSelectedItemName(){ return slots[selectedSlot].getItemName(); }
Item* Inventory::getCurrentSelectedItem(){ return slots[selectedSlot].getHeldItem(); }
void Inventory::removeItemFromSelected(int amount){ slots[selectedSlot].incrementItemCount(-amount); }

bool Inventory::AddItem(Item* newItem){
    for(auto& slot : slots){
        if (slot.getOccupied() && slot.getItemName() == newItem->name){
            slot.incrementItemCount(newItem->count);
            delete newItem;
            return true;
        }
    }
    for (auto& slot: slots){
        if(!slot.getOccupied()){
            slot.SetItem(newItem);
            return true;
        }
    }
    return false;
}

void Inventory::DrawInventory(int screenW, int screenH){
    if (isOpened){
        DrawRectangle(0, 0, screenW, screenH, ColorAlpha(DARKGRAY, 0.75f));
        for (auto& slot : slots) slot.DrawSlot();
        if(craftingMenu){
            for(auto& cSlot : craftingslots) cSlot.DrawSlot();
        }
    } else {
        for (int i = 0; i < cols; i++){
            slots[i].DrawSlot();
            highlightSelectedSlot();
        }
    }
}

void Inventory::highlightSelectedSlot(){
    if(selectedSlot >= 0 && selectedSlot < cols){
        Rectangle highlightSelectedSlot = slots[selectedSlot].getRec();
        DrawRectangleLinesEx(highlightSelectedSlot, 3.0f, WHITE);
    }
}