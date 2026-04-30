#include "World.h"
#include <cmath>
#include <raymath.h>
#include "ItemDrop.h" 
#include "Player.h"   
#include <random>


World::World() {}

World::~World() {
    chunks.clear();
}


std::pair<int, int> World::GetChunkCoords(int x, int y, int tileSize) {
    int chunkSizePixels = CHUNK_SIZE * tileSize;
    int cx = floor((float)x / chunkSizePixels);
    int cy = floor((float)y / chunkSizePixels);
    return {cx, cy};
}

void World::AddTile(TileDef def, int x, int y, int tileSize) {
    auto coords = GetChunkCoords(x, y, tileSize);
    chunks[coords].tiles.push_back(CreateTileFromBlueprint(def, x, y, tileSize));
}

void World::RemoveTileAt(int worldX, int worldY, int tileSize) {
    auto coords = GetChunkCoords(worldX, worldY, tileSize);
    
    if (chunks.count(coords)) {
        auto& v = chunks[coords].tiles;
        for (int i = 0; i < v.size(); i++) {
            if ((int)v[i].getPos().x == worldX && (int)v[i].getPos().y == worldY) {
                v.erase(v.begin() + i);
                break;
            }
        }
    }
}

void World::Generate(int tileSize, int worldW, int worldH) {
    float seed = (float)GetRandomValue(1, 10000);

    for (int x = 0; x < worldW; x++) {
        float mountains = Noise1D(x * 0.05f, seed) * 30.0f;
        float jagged = Noise1D(x * 0.2f, seed + 100) * 5.0f;
        int surfaceY = 15 + (int)(mountains + jagged);

        for (int y = surfaceY; y < worldH; y++) {
            float depth = (float)(y - surfaceY) / (float)(worldH - surfaceY);
            float caveShape = Noise2D(x * 0.05f, y * 0.05f, seed + 500);
            float caveThreshold = 0.1f + (depth * 0.1f);

            if (fabs(caveShape) >= caveThreshold) {
                TileDef tile;
                if (surfaceY < 5) tile = {"SNOW", "SNOW", "SNOW", 1, true, TileShape::FULL_BLOCK};
                else if (y == surfaceY) tile = {"GRASS", "GRASS", "GRASS", 1, true, TileShape::FULL_BLOCK};
                else if (y < surfaceY + 4) tile = {"DIRT", "DIRT", "DIRT", 1, true, TileShape::FULL_BLOCK};
                else {
                    float oreNoise = Noise2D(x * 0.6f, y * 0.6f, seed + 999);
                    float stoneNoise = Noise2D(x * 0.5f, y * 0.5f, seed + 777);

                    if (y > 100 && oreNoise > 0.8f) tile = {"AMBERORE", "AMBERORE", "AMBERORE", 1, true, TileShape::FULL_BLOCK};
                    else if (y > 30 && oreNoise > 0.88f) tile = {"COPPARORE", "COPPARORE", "COPPARORE", 1, true, TileShape::FULL_BLOCK};
                    else if (y > 60 && oreNoise > 0.75f) tile = {"IRONORE", "IRONORE", "IRONORE", 1, true, TileShape::FULL_BLOCK};
                    else if (stoneNoise > 0.85f) tile = {"COBBLESTONE", "COBBLESTONE", "COBBLESTONE", 1, true, TileShape::FULL_BLOCK};
                    else tile = {"STONE", "STONE", "COBBLESTONE", 1, true, TileShape::FULL_BLOCK};
                }
                AddTile(tile, x * tileSize, y * tileSize, tileSize);
            }
        }
    }
}

void World::Draw(Camera2D camera, int screenW, int screenH, int tileSize) {

    float viewLeft = camera.target.x - (camera.offset.x / camera.zoom);
    float viewTop = camera.target.y - (camera.offset.y / camera.zoom);
    float viewRight = viewLeft + (screenW / camera.zoom);
    float viewBottom = viewTop + (screenH / camera.zoom);

    int chunkSizePixels = CHUNK_SIZE * tileSize;
    
    int startCX = floor(viewLeft / chunkSizePixels);
    int endCX = floor(viewRight / chunkSizePixels);
    int startCY = floor(viewTop / chunkSizePixels);
    int endCY = floor(viewBottom / chunkSizePixels);

    for (int cx = startCX; cx <= endCX; cx++) {
        for (int cy = startCY; cy <= endCY; cy++) {
            if (chunks.count({cx, cy})) {
                for (Tile &t : chunks[{cx, cy}].tiles) {
                    t.Draw();
                }
            }
        }
    }
}

Tile World::CreateTileFromBlueprint(TileDef def, int x, int y, int tileSize) {
    return Tile((float)x, (float)y, (float)tileSize, (float)tileSize, def);
}

float World::Noise1D(float x, float seed) {
    return (sin(x + seed) + sin(x * 0.5f + seed * 1.5f)) * 0.5f;
}

float World::Noise2D(float x, float y, float seed) {
    return (sin(x + seed) + cos(y + seed)) * 0.5f;
}


void World::Explode(Vector2 center, float radius, int tileSize) {
    for (auto& chunkEntry : chunks) {
        auto& tiles = chunkEntry.second.tiles;
        for (auto it = tiles.begin(); it != tiles.end(); ) {
            
            Vector2 tilePos = it->getPos();
            float dist = Vector2Distance(center, tilePos);
            
            if (dist <= radius) {
                Item* drop = it->CreateDrop();
                if (drop) {
                    float dirX = tilePos.x - center.x;
                    float dirY = tilePos.y - center.y;
                    float force = 40.0f; 
                    float vx = (dirX / (dist + 0.1f)) * force;
                    float vy = (dirY / (dist + 0.1f)) * force;
                    vy -= 5.0f;

                    SpawnPhysicalDrop(drop, tilePos.x, tilePos.y, vx, vy);
                }

                it = tiles.erase(it);
            } else {
                it++;
            }
        }
    }


    for (auto* item : droppedItems) {
        Vector2 itemPos = { item->GetRect().x + item->GetRect().width / 2.0f, 
                            item->GetRect().y + item->GetRect().height / 2.0f };
        
        float dist = Vector2Distance(center, itemPos);

        if (dist <= radius) {
            float dirX = itemPos.x - center.x;
            float dirY = itemPos.y - center.y;
            
            float force = 40.0f;
            float vx = (dirX / (dist + 0.1f)) * force;
            float vy = (dirY / (dist + 0.1f)) * force;
            vy -= 5.0f; 

            item->SetVelocity({ vx, vy });
        }
    }
}

void World::AddDroppedItem(ItemDrop* item) {
    droppedItems.push_back(item);
}

void World::UpdateItems(Player& player, float dt, int tileSize) {
    for (auto it = droppedItems.begin(); it != droppedItems.end();) {
        ItemDrop* item = *it;
        item->Update(*this, tileSize, dt);

      
        if (item->isExpired()) {
            delete item;
            it = droppedItems.erase(it);
            continue; 
        }
        if (CheckCollisionRecs(player.GetRect(), item->GetRect())) {
            if (player.getInventory().AddItem(item->itemData)) {
                item->itemData = nullptr;
                delete item;
                it = droppedItems.erase(it);
                continue;
            }
        }
        
        it++;
    }
}

void World::DrawItems() {
    for (auto* item : droppedItems) {
        if (item != nullptr) {
            item->Draw(); 
        }
    }
}

void World::SpawnPhysicalDrop(Item* data, float x, float y, int velX, int velY) {
    if (!data) return;
    

    ItemDrop* newDrop = new ItemDrop(data, x, y, velX, velY);
    droppedItems.push_back(newDrop);
}