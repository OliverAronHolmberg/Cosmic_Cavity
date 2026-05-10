#pragma once
#include "Inventory.h"

class CraftingUI {
protected:
    bool isOpened = false;
    std::vector<InventorySlot> craftingslots;
    Item* mouseItem = nullptr;

public:
    virtual ~CraftingUI() {}
    
    virtual void Toggle() {
        isOpened = !isOpened;
        if (!isOpened) {
            ClearItems();
        }
    }
    
    virtual void ClearItems() {
        for (auto& slot : craftingslots) {
            if (slot.getOccupied()) {
                slot.SetHeldItemRaw(nullptr);
            }
        }
    }
    
    virtual bool IsOpen() const { return isOpened; }
    
    virtual void SetOpen(bool open) { isOpened = open; }
    
    virtual std::vector<InventorySlot>& GetCraftingSlots() { return craftingslots; }
    
    virtual void HandleInput(Vector2 mousePos) {
        for (int i = 0; i < (int)craftingslots.size(); i++) {
            InventorySlot& slot = craftingslots[i];
            Rectangle slotRec = slot.getRec();
            
            if (CheckCollisionPointRec(mousePos, slotRec)) {
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
        
        if (mouseItem && mouseItem->count <= 0) {
            delete mouseItem;
            mouseItem = nullptr;
        }
    }
    
    virtual Item* CheckRecipe(RecipeManager& rm) = 0;
    virtual void Draw(int screenW, int screenH, RecipeManager& rm) = 0;
    virtual void OnInteract(Inventory& playerInv) = 0;
};

class CrafterUI : public CraftingUI {
public:
    void Setup(int centerX, int centerY) {
        int craftX = centerX + 350;
        int craftY = centerY + 100;
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                craftingslots.emplace_back(craftX + (c * 60), craftY + (r * 60), 50, 50);
            }
        }
        craftingslots.emplace_back(craftX + 220, craftY + 60, 60, 60);
    }
    
    Item* CheckRecipe(RecipeManager& rm) override {
        bool hasItems = false;
        for (int i = 0; i < 9; i++) {
            if (craftingslots[i].getOccupied() && craftingslots[i].getHeldItem()) {
                hasItems = true;
                break;
            }
        }
        if (!hasItems) return nullptr;
        
        return rm.CheckGridRecipe(craftingslots);
    }
    
    void OnInteract(Inventory& playerInv) override {
        Toggle();
    }
    
    void Draw(int screenW, int screenH, RecipeManager& rm) override {
        if (!isOpened) return;
        
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
            }
        }
        
        DrawText("->", menuX + 170, menuY + 70, 30, WHITE);
        
        Item* recipeResult = CheckRecipe(rm);
        
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
                       
                    }
                }
            }
        } else {
            DrawRectangleLinesEx(resultRec, 2, DARKGRAY);
        }
        
        if (recipeResult) delete recipeResult;
    }
};

class FurnaceUI : public CraftingUI {
public:
    void Setup(int centerX, int centerY) {
        int furnaceX = centerX + 350;
        int furnaceY = centerY + 100;
        
        craftingslots.emplace_back(furnaceX, furnaceY, 50, 50);
        craftingslots.emplace_back(furnaceX + 60, furnaceY, 50, 50);
        craftingslots.emplace_back(furnaceX + 120, furnaceY + 25, 60, 60);
    }
    
    Item* CheckRecipe(RecipeManager& rm) override {
        return nullptr;
    }
    
    void OnInteract(Inventory& playerInv) override {
        Toggle();
    }
    
    void Draw(int screenW, int screenH, RecipeManager& rm) override {
        if (!isOpened) return;
        
        int menuX = screenW - 400;
        int menuY = 150;
        
        DrawRectangle(menuX - 20, menuY - 40, 250, 180, ColorAlpha(DARKGRAY, 0.9f));
        DrawText("Furnace", menuX, menuY - 30, 24, WHITE);
        
        for (int i = 0; i < (int)craftingslots.size(); i++) {
            InventorySlot& slot = craftingslots[i];
            Rectangle slotRec = slot.getRec();
            
            DrawRectangleRec(slotRec, ColorAlpha(LIGHTGRAY, 0.5f));
            DrawRectangleLinesEx(slotRec, 2, DARKGRAY);
            
            if (slot.getOccupied() && slot.getHeldItem()) {
                Item* item = slot.getHeldItem();
                DrawTextureScaled(textureAssets.Get(item->textureID), { slotRec.x + 5, slotRec.y + 5, slotRec.width - 10, slotRec.height - 10 });
            }
        }
    }
};