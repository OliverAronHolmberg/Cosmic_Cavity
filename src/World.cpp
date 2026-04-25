#include "World.h"
#include <cmath>

World::World() {
    // Constructor logic if needed
}

World::~World() {
    // Tiles are stored by value to reduce fragmentation and allocations
    tiles.clear();
}

Tile World::CreateTileFromID(std::string blockID, int x, int y, int tileSize) {
    bool hasCollision = true;
    std::string dropItem = blockID;
    return Tile(x, y, tileSize, tileSize, blockID, blockID, dropItem, 1, true, hasCollision);
}

// Fixed GetTileAt: This is used for removal and collision
Tile World::GetTileAt(int worldX, int worldY) {
    for (Tile &t : tiles) {
        Rectangle r = t.getRec();
        if (CheckCollisionPointRec({(float)worldX, (float)worldY}, r)) {
            return t;
        }
    }
    // Return a dummy tile (non-existent)
    return Tile(0, 0, 0, 0, "NONE", "NONE", "NONE", 0, false, false);
}

// Fixed RemoveTileAt: Use this in your main loop when clicking
void World::RemoveTileAt(int index) {
    if (index >= 0 && index < (int)tiles.size()) {
        tiles.erase(tiles.begin() + index);
    }
}

// Empty shell to prevent errors
void World::CreateStructure(int startX, int startY, int tileSize) {
    // Logic for trees/houses goes here later
}

void World::Generate(int tileSize, int worldW, int worldH) {
    float seed = (float)GetRandomValue(1, 10000);
    float caveSize = 0.05f;
    
    for (int x = 0; x < worldW; x++) {
        float mountains = Noise1D(x * 0.05f, seed) * 30.0f;
        float jagged = Noise1D(x * 0.2f, seed + 100) * 5.0f;
        int surfaceY = 30 + (int)(mountains + jagged); // Lowered surface for visibility

        for (int y = surfaceY; y < worldH; y++) {
            float depth = (float)(y - surfaceY) / (float)(worldH - surfaceY);
            float caveShape = Noise2D(x * caveSize, y * caveSize, seed + 500);
            float caveThreshold = 0.1f + (depth * 0.1f); 

            if (fabs(caveShape) > caveThreshold) {
                std::string name = "STONE";
                if (y == surfaceY) name = "GRASS";
                else if (y < surfaceY + 4) name = "DIRT";

                tiles.emplace_back(x * tileSize, y * tileSize, tileSize, tileSize, name, name, name, 1, true, true);
            }
        }
    }
}

float World::Noise1D(float x, float seed) {
    return (sin(x + seed) + sin(x * 0.5f + seed * 1.5f)) * 0.5f;
}

float World::Noise2D(float x, float y, float seed) {
    return (sin(x + seed) + cos(y + seed)) * 0.5f;
}

void World::Draw(Camera2D camera, int screenW, int screenH, int tileSize) {
    float viewLeft = camera.target.x - (camera.offset.x / camera.zoom);
    float viewTop = camera.target.y - (camera.offset.y / camera.zoom);
    float viewRight = viewLeft + (screenW / camera.zoom);
    float viewBottom = viewTop + (screenH / camera.zoom);
    int margin = tileSize * 2;

        for (Tile &t : tiles) {
        Rectangle rec = t.getRec();
        if (rec.x + rec.width > viewLeft - margin &&
            rec.x < viewRight + margin &&
            rec.y + rec.height > viewTop - margin &&
            rec.y < viewBottom + margin) 
        {
            t.DrawTile();
        }
    }
}
