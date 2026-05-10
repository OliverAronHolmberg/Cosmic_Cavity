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
    if (isOccupied && heldItem && heldItem->count <= 0) {
        delete heldItem;
        heldItem = nullptr;
        isOccupied = false;
    }
}

void InventorySlot::DrawSlot() {
    DrawTextureScaled(textureAssets.Get(bgTextureID), getRec());
    if (isOccupied && heldItem && heldItem->count > 0) {
        Rectangle itemRec = {(float)x + 10, (float)y + 10, (float)w - 20, (float)h - 20};
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
            slots.emplace_back(X + (c * slotSize), Y - (r * slotSize), slotSize, slotSize);
        }
    }

    int craftX = X + 350;
    int craftY = Y + 100;
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            craftingslots.emplace_back(craftX + (c * 60), craftY + (r * 60), 50, 50);
        }
    }
    craftingslots.emplace_back(craftX + 220, craftY + 60, 60, 60);
}

Inventory::~Inventory() {
    if (mouseItem) delete mouseItem;
}

void Inventory::UpdateMouseLogic(Container* activeContainer) {
    if (!isOpened || !activeContainer) return;

    Vector2 mPos = GetMousePosition();

    for (auto& slot : activeContainer->GetSlots()) {
        if (CheckCollisionPointRec(mPos, slot.getRec())) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Item* slotItem = slot.getHeldItem(); 

                if (mouseItem && slotItem && mouseItem->name == slotItem->name) {
                    int spaceLeft = slotItem->max - slotItem->count;
                    int transfer = (mouseItem->count < spaceLeft) ? mouseItem->count : spaceLeft;
                    
                    slotItem->count += transfer;
                    mouseItem->count -= transfer;
                } else {
                    slot.SwapItem(mouseItem);
                }
            }
            

            else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                Item* slotItem = slot.getHeldItem(); 

                if (!mouseItem && slotItem) { 
                    int half = slotItem->count / 2;
                    if (half == 0) half = 1; 

                    mouseItem = slotItem->Clone();
                    mouseItem->count = half;
                    slotItem->count -= half;
                } 
                else if (mouseItem) { 
                    if (!slotItem) { 
                        slot.SetItem(mouseItem->Clone());
                        slot.getHeldItem()->count = 1;
                        mouseItem->count--;
                    } 
                    else if (slotItem->name == mouseItem->name && slotItem->count < slotItem->max) { 
                        slotItem->count++;
                        mouseItem->count--;
                    }
                }
            }

            
            slot.UpdateSlotState();
            break;
        }
    }

    
    if (mouseItem && mouseItem->count <= 0) {
        delete mouseItem;
        mouseItem = nullptr;
    }
}



void Inventory::DrawMouseItem() {
    if (mouseItem && mouseItem->count > 0) {
        Vector2 mPos = GetMousePosition();
        Rectangle drawRec = { mPos.x - 30, mPos.y - 30, 60, 60 };
        DrawTextureScaled(textureAssets.Get(mouseItem->textureID), drawRec);
        DrawText(TextFormat("%i", mouseItem->count), mPos.x-30, mPos.y-30, 20, WHITE);
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


void Inventory::DrawCraftingMenu(RecipeManager& rm, int screenW, int screenH) {
    if (!isOpened || !craftingMenu) return;

    int menuX = screenW - 400;
    int menuY = 150;
    int slotSize = 50;
    int resultSlotX = menuX + 220;
    int resultSlotY = menuY + slotSize;
    
    DrawRectangle(menuX - 20, menuY - 40, 350, 250, ColorAlpha(DARKGRAY, 0.9f));
    DrawText("Crafting", menuX, menuY - 30, 24, WHITE);

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            int idx = r * 3 + c;
            Rectangle slotRec = { (float)menuX + (c * (slotSize + 5)), (float)menuY + (r * (slotSize + 5)), (float)slotSize, (float)slotSize };
            
            DrawRectangleRec(slotRec, ColorAlpha(LIGHTGRAY, 0.5f));
            DrawRectangleLinesEx(slotRec, 2, DARKGRAY);
            
            InventorySlot& slot = craftingslots[idx];
            if (slot.getOccupied() && slot.getHeldItem()) {
                Item* item = slot.getHeldItem();
                DrawTextureScaled(textureAssets.Get(item->textureID), { slotRec.x + 5, slotRec.y + 5, slotRec.width - 10, slotRec.height - 10 });
                if (item->count > 1) {
                    DrawText(TextFormat("%d", item->count), slotRec.x + 5, slotRec.y + 5, 16, WHITE);
                }
            }
            
            if (CheckCollisionPointRec(GetMousePosition(), slotRec)) {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    Item* slotItem = slot.getHeldItem(); 
                    
                    if (mouseItem && slotItem && mouseItem->name == slotItem->name) {
                        int spaceLeft = slotItem->max - slotItem->count;
                        int transfer = (mouseItem->count < spaceLeft) ? mouseItem->count : spaceLeft;
                        slotItem->count += transfer;
                        mouseItem->count -= transfer;
                    } else {
                        slot.SwapItem(mouseItem);
                    }
                }
                
                if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                    Item* slotItem = slot.getHeldItem(); 
                    
                    if (!mouseItem && slotItem) { 
                        int half = slotItem->count / 2;
                        if (half == 0) half = 1; 
                        mouseItem = slotItem->Clone();
                        mouseItem->count = half;
                        slotItem->count -= half;
                    } 
                    else if (mouseItem) { 
                        if (!slotItem) { 
                            slot.SetItem(mouseItem->Clone());
                            slot.getHeldItem()->count = 1;
                            mouseItem->count--;
                        } 
                        else if (slotItem->name == mouseItem->name && slotItem->count < slotItem->max) { 
                            slotItem->count++;
                            mouseItem->count--;
                        }
                    }
                }
            }
            
            slot.UpdateSlotState();
        }
    }
    
    DrawText("->", menuX + 170, menuY + 70, 30, WHITE);
    
    Item* recipeResult = rm.CheckGridRecipe(craftingslots);
    
    Rectangle resultRec = { (float)resultSlotX, (float)resultSlotY, 60, 60 };
    DrawRectangleRec(resultRec, ColorAlpha(LIGHTGRAY, 0.5f));
    
    if (recipeResult) {
        DrawRectangleLinesEx(resultRec, 3, GOLD);
        
        DrawTextureScaled(textureAssets.Get(recipeResult->textureID), { resultRec.x + 5, resultRec.y + 5, resultRec.width - 10, resultRec.height - 10 });
        if (recipeResult->count > 1) {
            DrawText(TextFormat("%d", recipeResult->count), resultRec.x + 5, resultRec.y + 5, 16, WHITE);
        }
        
        if (CheckCollisionPointRec(GetMousePosition(), resultRec)) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                int craftCount = 1;
                if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                    craftCount = 999;
                }
                for (int c = 0; c < craftCount; c++) {
                    AddItem(recipeResult->Clone());
                    for (int i = 0; i < 9; i++) {
                        InventorySlot& slot = craftingslots[i];
                        if (slot.getOccupied() && slot.getHeldItem()) {
                            slot.getHeldItem()->count--;
                            slot.UpdateSlotState();
                        }
                    }
                    recipeResult = rm.CheckGridRecipe(craftingslots);
                    if (!recipeResult) break;
                }
                delete recipeResult;
                recipeResult = nullptr;
            }
            
            else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                if (!mouseItem) {
                    mouseItem = recipeResult->Clone();
                }
            }
        }
    } else {
        DrawRectangleLinesEx(resultRec, 2, DARKGRAY);
    }
    
    if (recipeResult) delete recipeResult;
    
    if (mouseItem && mouseItem->count <= 0) {
        delete mouseItem;
        mouseItem = nullptr;
    }
}

void Inventory::highlightSelectedSlot() {
    Rectangle r = slots[selectedSlot].getRec();
    DrawRectangleLinesEx(r, 3.0f, WHITE);
}


bool Inventory::HasAmount(std::string itemName, int amount) {
    int total = 0;
    for (const auto& slot : slots) {
        if (slot.getOccupied() && slot.getHeldItem()->name == itemName) {
            total += slot.getHeldItem()->count;
        }
    }
    return total >= amount;
}

void Inventory::RemoveItems(std::string itemName, int amount) {
    for (auto& slot : slots) {
        if (amount <= 0) break;
        Item* item = slot.getHeldItem();
        if (slot.getOccupied() && item->name == itemName) {
            if (item->count > amount) {
                item->count -= amount;
                amount = 0;
            } else {
                amount -= item->count;
                item->count = 0;
                slot.UpdateSlotState(); 
            }
        }
    }
}


