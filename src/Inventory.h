#pragma once
#include <vector>
#include <string>
#include <raylib.h>
#include "Items.h"
#include "Tiles.h"
#include "TextureHandler.h"


class Item;
class InventorySlot;
struct Recipe; 
class RecipeManager;

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
    bool craftingMenu = false;

    Inventory(int X, int Y, int W, int H);
    ~Inventory();

    void ToggleInventory() { 
        isOpened = !isOpened; 
        if (isOpened) craftingMenu = false;
        if (!isOpened) {
            for (auto& slot : craftingslots) {
                if (slot.getOccupied()) {
                    AddItem(slot.getHeldItem());
                    slot.SetHeldItemRaw(nullptr);
                }
            }
        }
    }
    void setSelectedSlot(int slotNumber);
    
    Item* getCurrentSelectedItem();
    void updateActiveSlot(); 

    std::vector<InventorySlot>& GetSlots() override { return slots; }
    
    void UpdateMouseLogic(Container* activeContainer);
    void DrawMouseItem();
    

    bool AddItem(Item* newItem);
    void DrawInventory(int screenW, int screenH);

    bool HasAmount(std::string itemName, int amount);
    void RemoveItems(std::string itemName, int amount);
    void DrawCraftingMenu(RecipeManager& rm, int screenW, int screenH);
    void ClearCraftingGrid() {
        for (auto& slot : craftingslots) {
            if (slot.getOccupied()) {
                AddItem(slot.getHeldItem());
                slot.SetHeldItemRaw(nullptr);
            }
        }
    }

private:
    void highlightSelectedSlot();
};


class RecipeManager {
public:
    std::vector<Recipe> recipes;

RecipeManager() {
        Recipe stoneWallRecipe = {"STONEWALL", "STONEWALL", 1, {}, true, TileShape::WALL};
        stoneWallRecipe.grid[0] = "STONE"; stoneWallRecipe.grid[1] = "STONE"; stoneWallRecipe.grid[2] = "";
        stoneWallRecipe.grid[3] = ""; stoneWallRecipe.grid[4] = ""; stoneWallRecipe.grid[5] = "";
        stoneWallRecipe.grid[6] = ""; stoneWallRecipe.grid[7] = ""; stoneWallRecipe.grid[8] = "";
        recipes.push_back(stoneWallRecipe);

        Recipe cobbleStoneWallRecipe = {"COBBLESTONEWALL", "COBBLESTONEWALL", 1, {}, true, TileShape::WALL};
        cobbleStoneWallRecipe.grid[0] = "COBBLESTONE"; cobbleStoneWallRecipe.grid[1] = "COBBLESTONE"; cobbleStoneWallRecipe.grid[2] = "";
        cobbleStoneWallRecipe.grid[3] = ""; cobbleStoneWallRecipe.grid[4] = ""; cobbleStoneWallRecipe.grid[5] = "";
        cobbleStoneWallRecipe.grid[6] = ""; cobbleStoneWallRecipe.grid[7] = ""; cobbleStoneWallRecipe.grid[8] = "";
        recipes.push_back(cobbleStoneWallRecipe);
        
        Recipe workbenchRecipe = {"CRAFTER", "CRAFTER", 1, {}, true, TileShape::FULL_BLOCK};
        workbenchRecipe.grid[0] = "STEELBLOCK"; workbenchRecipe.grid[1] = "DATABOX"; workbenchRecipe.grid[2] = "STEELBLOCK";
        workbenchRecipe.grid[3] = "STEELBLOCK"; workbenchRecipe.grid[4] = "STEELBLOCK"; workbenchRecipe.grid[5] = "STEELBLOCK";
        workbenchRecipe.grid[6] = ""; workbenchRecipe.grid[7] = ""; workbenchRecipe.grid[8] = "";
        recipes.push_back(workbenchRecipe);

        Recipe steelBlock = {"STEELBLOCK", "STEELBLOCK", 1, {}, true, TileShape::FULL_BLOCK};
        steelBlock.grid[0] = "STEEL"; steelBlock.grid[1] = "STEEL"; steelBlock.grid[2] = "STEEL";
        steelBlock.grid[3] = "STEEL"; steelBlock.grid[4] = "STEEL"; steelBlock.grid[5] = "STEEL";
        steelBlock.grid[6] = "STEEL"; steelBlock.grid[7] = "STEEL"; steelBlock.grid[8] = "STEEL";
        recipes.push_back(steelBlock);

        Recipe steel = {"STEEL", "STEEL", 9, {}, true, TileShape::FULL_BLOCK};
        steel.grid[0] = "STEELBLOCK"; steel.grid[1] = ""; steel.grid[2] = "";
        steel.grid[3] = ""; steel.grid[4] = ""; steel.grid[5] = "";
        steel.grid[6] = ""; steel.grid[7] = ""; steel.grid[8] = "";
        recipes.push_back(steel);


        Recipe fusionReactor = {"FUSIONREACTOR", "FUSIONREACTOR", 1, {}, true, TileShape::FULL_BLOCK};
        fusionReactor.grid[0] = "STEEL"; fusionReactor.grid[1] = "STEEL"; fusionReactor.grid[2] = "STEEL";
        fusionReactor.grid[3] = "STEEL"; fusionReactor.grid[4] = "FUSIONCORE"; fusionReactor.grid[5] = "STEEL";
        fusionReactor.grid[6] = "STEEL"; fusionReactor.grid[7] = "STEEL"; fusionReactor.grid[8] = "STEEL";
        recipes.push_back(fusionReactor);

        Recipe furnace = {"FURNACE", "FURNACE", 1, {}, true, TileShape::FULL_BLOCK};
        furnace.grid[0] = "STEELBLOCK"; furnace.grid[1] = "STEELBLOCK"; furnace.grid[2] = "STEELBLOCK";
        furnace.grid[3] = "STEELBLOCK"; furnace.grid[4] = ""; furnace.grid[5] = "STEELBLOCK";
        furnace.grid[6] = "STEELBLOCK"; furnace.grid[7] = "STEELBLOCK"; furnace.grid[8] = "STEELBLOCK";
        recipes.push_back(furnace);

        

    }

   
    bool CanCraft(const Recipe& recipe, Inventory& inv) {
        for (const auto& ing : recipe.ingredients) {
            if (!inv.HasAmount(ing.id, ing.amount)) {
                return false;
            }
        }
        return true;
    }
    
    

    Item* CheckGridRecipe(std::vector<InventorySlot>& allSlots) {
        bool hasItems = false;
        for (int i = 0; i < 9; i++) {
            if (allSlots[i].getOccupied() && allSlots[i].getHeldItem()) {
                hasItems = true;
                break;
            }
        }
        if (!hasItems) return nullptr;
        
        for (auto& recipe : recipes) {
            bool match = true;
            for (int i = 0; i < 9; i++) {
                std::string slotItemName = "";
                if (allSlots[i].getOccupied() && allSlots[i].getHeldItem()) {
                    slotItemName = allSlots[i].getHeldItem()->name;
                }
                
                if (recipe.grid[i].empty()) {
                    if (!slotItemName.empty()) { match = false; break; }
                } else {
                    if (slotItemName != recipe.grid[i]) { match = false; break; }
                }
            }
            
            if (match) {
                if (recipe.isBlock) {
                    TileDef def;
                    def.name = recipe.resultID;
                    def.textureID = recipe.resultTexture;
                    def.dropID = recipe.resultID;
                    def.dropAmount = recipe.resultAmount;
                    def.hasCollision = true;
                    def.shape = recipe.shape;
                    return new BlockItem(recipe.resultID, def, recipe.resultAmount);
                } else {
                    return new Item(recipe.resultID, recipe.resultTexture, recipe.resultAmount, 999, false);
                }
            }
        }
        return nullptr;
    }
};