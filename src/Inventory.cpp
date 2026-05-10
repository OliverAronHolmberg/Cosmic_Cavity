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

    int totalWidth = cols * slotSize;
    int centeredX = (GetScreenWidth() - totalWidth) / 2;



    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            slots.emplace_back(centeredX + (c * slotSize), Y - (r * slotSize), slotSize, slotSize);
        }
    }

    int craftX = centeredX;
    int craftY = Y - 450;
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            craftingslots.emplace_back(craftX + (c * 60), craftY + (r * 60), 50, 50);
        }
    }
    craftingslots.emplace_back(craftX + 200, craftY + 60, 60, 60);
    
    int furnaceX = centeredX;
    int furnaceY = Y - 450;
    furnaceslots.emplace_back(furnaceX, furnaceY, 50, 50);
    furnaceslots.emplace_back(furnaceX + 60, furnaceY, 50, 50);
    furnaceslots.emplace_back(furnaceX + 140, furnaceY + 25, 60, 60);
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
                    if (spaceLeft > 0) {
                        int transfer = (mouseItem->count < spaceLeft) ? mouseItem->count : spaceLeft;
                        slotItem->count += transfer;
                        mouseItem->count -= transfer;
                    }
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
                        slot.SetItem(mouseItem);
                        mouseItem = nullptr;
                    } 
                    else if (slotItem->name == mouseItem->name) { 
                        int space = slotItem->max - slotItem->count;
                        if (space > 0) {
                            int toAdd = (mouseItem->count < space) ? mouseItem->count : space;
                            slotItem->count += toAdd;
                            mouseItem->count -= toAdd;
                        }
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

        for (auto& slot : slots) {
            slot.DrawSlot();
        }

    } else {
        for (int i = 0; i < cols; i++) slots[i].DrawSlot();
        highlightSelectedSlot();
    }
}


void Inventory::DrawCraftingMenu(RecipeManager& rm, int screenW, int screenH) {
    if (!isOpened || !craftingMenu) return;

    int totalInvWidth = 9 * 75; 
    int inventoryLeftEdge = (screenW - totalInvWidth) / 2;
    
    int menuX = inventoryLeftEdge + 20;


    int inventoryTopEdge = screenH - (5 * 75);
    int rectY = inventoryTopEdge - 250 -10;


    int menuY = rectY + 40;
    int slotSize = 50;
    int resultSlotX = menuX + 220;
    int resultSlotY = menuY + slotSize;
    
   
    DrawRectangle(inventoryLeftEdge, rectY, 350, 250, ColorAlpha(DARKGRAY, 0.9f));
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

void Inventory::DrawFurnaceMenu(RecipeManager& rm, int screenW, int screenH) {
    if (!isOpened || !furnaceOpen) return;
    
    int totalInvWidth = 9 * 75; 
    int inventoryLeftEdge = (screenW - totalInvWidth) / 2;
    
    int menuX = inventoryLeftEdge + 20;


    int inventoryTopEdge = screenH - (5 * 75);
    int rectY = inventoryTopEdge - 250 - 10;


    int menuY = rectY + 80;
    int slotSize = 50;
    int resultSlotX = menuX + 220;
    int resultSlotY = menuY + slotSize;
    
   
    
    DrawRectangle(menuX - 20, menuY, 250, 180, ColorAlpha(DARKGRAY, 0.9f));
    DrawText("Furnace", menuX, menuY - 30, 24, WHITE);
    
    DrawText("Fuel", menuX, menuY, 16, LIGHTGRAY);
    DrawText("Input", menuX + 60, menuY, 16, LIGHTGRAY);
    DrawText("Output", menuX + 130, menuY + 10, 16, LIGHTGRAY);

    Item* fuelItem = furnaceslots[0].getHeldItem();
    Item* inputItem = furnaceslots[1].getHeldItem();
    Item* outputItem = furnaceslots[2].getHeldItem();

    FurnaceRecipe* smeltRecipe = nullptr;
    bool hasFuel = false;
    int burnTime = 0;

    if (fuelItem) {
        hasFuel = rm.IsFuelItem(fuelItem->name);
        if (hasFuel) {
            burnTime = rm.GetFuelBurnTimeItem(fuelItem->name);
        }
    }

    if (inputItem) {
        smeltRecipe = rm.FindFurnaceRecipe(inputItem->name);
    }

    if (smeltRecipe && hasFuel && !outputItem && !mouseItem) {
        Rectangle smeltBtn = { menuX + 140, menuY + 80, 80, 30 };
        DrawRectangleRec(smeltBtn, ColorAlpha(ORANGE, 0.8f));
        DrawText("Smelt", smeltBtn.x + 20, smeltBtn.y + 8, 16, WHITE);
        
        if (CheckCollisionPointRec(GetMousePosition(), smeltBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (inputItem->count >= 1) {
                inputItem->count--;
                if (inputItem->count <= 0) {
                    delete inputItem;
                    furnaceslots[1].SetHeldItemRaw(nullptr);
                }
                
                fuelItem->count--;
                if (fuelItem->count <= 0) {
                    delete fuelItem;
                    furnaceslots[0].SetHeldItemRaw(nullptr);
                }
                
                Item* result = new Item(smeltRecipe->resultID, smeltRecipe->resultTexture, smeltRecipe->resultAmount, 999);
                furnaceslots[2].SetItem(result);
            }
        }
    }

    if (burnTime > 0 && smeltRecipe) {
        DrawText(TextFormat("Burn: %d", burnTime), menuX, menuY + 120, 14, ORANGE);
    }
    
    for (int i = 0; i < (int)furnaceslots.size(); i++) {
        InventorySlot& slot = furnaceslots[i];
        Rectangle slotRec = slot.getRec();
        
        DrawRectangleRec(slotRec, ColorAlpha(LIGHTGRAY, 0.5f));
        DrawRectangleLinesEx(slotRec, 2, DARKGRAY);
        
        if (slot.getOccupied() && slot.getHeldItem()) {
            Item* item = slot.getHeldItem();
            DrawTextureScaled(textureAssets.Get(item->textureID), { slotRec.x + 5, slotRec.y + 5, slotRec.width - 10, slotRec.height - 10 });
            if (item->count > 1) {
                DrawText(TextFormat("%d", item->count), slotRec.x + 5, slotRec.y + 5, 16, WHITE);
            }
        }
        
        if (!CheckCollisionPointRec(GetMousePosition(), slotRec)) continue;
        
        bool isOutputSlot = (i == 2);
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Item* slotItem = slot.getHeldItem();
            if (mouseItem && slotItem && mouseItem->name == slotItem->name && !isOutputSlot) {
                int spaceLeft = slotItem->max - slotItem->count;
                int transfer = (mouseItem->count < spaceLeft) ? mouseItem->count : spaceLeft;
                slotItem->count += transfer;
                mouseItem->count -= transfer;
            } else if (!mouseItem && slotItem) {
                mouseItem = slotItem;
                slot.SetHeldItemRaw(nullptr);
            } else if (!slotItem && !isOutputSlot) {
                slot.SetItem(mouseItem);
                mouseItem = nullptr;
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
            else if (mouseItem && !slotItem && !isOutputSlot) {
                slot.SetItem(mouseItem);
                mouseItem = nullptr;
            }
            else if (mouseItem && slotItem && slotItem->name == mouseItem->name && !isOutputSlot) {
                int space = slotItem->max - slotItem->count;
                if (space > 0) {
                    int toAdd = (mouseItem->count < space) ? mouseItem->count : space;
                    slotItem->count += toAdd;
                    mouseItem->count -= toAdd;
                }
            }
        }
        slot.UpdateSlotState();
    }
    
    if (mouseItem && mouseItem->count <= 0) {
        delete mouseItem;
        mouseItem = nullptr;
    }
}


