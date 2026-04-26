#include "Inventory.h"



InventorySlot::InventorySlot(int X, int Y, int W, int H) 
    : x(X), y(Y), w(W), h(H), bgTextureID("INVENTORYSLOT") {}

InventorySlot::~InventorySlot() {
    if (heldItem) delete heldItem;
}


InventorySlot::InventorySlot(InventorySlot&& other) noexcept {
    heldItem = other.heldItem;
    other.heldItem = nullptr;
    isOccupied = other.isOccupied;
    x = other.x; y = other.y; w = other.w; h = other.h;
    bgTextureID = other.bgTextureID;
}

void InventorySlot::SetItem(Item* newItem) {
    if (heldItem) delete heldItem;
    heldItem = newItem;
    isOccupied = (heldItem != nullptr);
}

void InventorySlot::UpdateSlotState() {
    if (isOccupied && heldItem) {
        if (heldItem->count <= 0) {
            delete heldItem;
            heldItem = nullptr;
            isOccupied = false;
        }
    }
}

void InventorySlot::DrawSlot() {
    DrawTextureScaled(textureAssets.Get(bgTextureID), getRec());
    if (isOccupied && heldItem && heldItem->count > 0) {
        Rectangle itemRec = {(float)x + 5, (float)y + 5, (float)w - 10, (float)h - 10};
        DrawTextureScaled(textureAssets.Get(heldItem->textureID), itemRec);
        DrawText(TextFormat("%i", heldItem->count), x + 5, y + 5, 20, WHITE);
    }
}



Inventory::Inventory(int X, int Y, int W, int H) : x(X), y(Y), w(W), h(H) {
    rows = 5; cols = 9;
    selectedSlot = 0;
    int slotSize = 75;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int sX = X + (c * slotSize);
            int sY = Y - (r * slotSize);
            slots.emplace_back(sX, sY, slotSize, slotSize);
        }
    }
}

void Inventory::setSelectedSlot(int slotNumber) {
    if (slotNumber >= 1 && slotNumber <= cols) selectedSlot = slotNumber - 1;
}

Item* Inventory::getCurrentSelectedItem() {
    slots[selectedSlot].UpdateSlotState(); 
    return slots[selectedSlot].getHeldItem();
}

void Inventory::updateActiveSlot() {
    slots[selectedSlot].UpdateSlotState();
}

bool Inventory::AddItem(Item* newItem) {
    if (!newItem) return false;

    for (auto& slot : slots) {
        Item* held = slot.getHeldItem();
        if (slot.getOccupied() && held && held->name == newItem->name) {
            held->count += newItem->count;
            delete newItem;
            return true;
        }
    }
   
    for (auto& slot : slots) {
        if (!slot.getOccupied()) {
            slot.SetItem(newItem);
            return true;
        }
    }
    return false;
}

void Inventory::DrawInventory(int screenW, int screenH) {
    if (isOpened) {
        DrawRectangle(0, 0, screenW, screenH, ColorAlpha(DARKGRAY, 0.75f));
        for (auto& slot : slots) slot.DrawSlot();
    } else {
      
        for (int i = 0; i < cols; i++) slots[i].DrawSlot();
        highlightSelectedSlot();
    }
}

void Inventory::highlightSelectedSlot() {
    Rectangle r = slots[selectedSlot].getRec();
    DrawRectangleLinesEx(r, 3.0f, WHITE);
}