#include "World.h"
#include <cmath>
#include <raymath.h>

World::World() {}

World::~World() {
    chunks.clear();
}

// Converts a raw pixel position into chunk grid coordinates
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
            // Check if the tile center or corner matches the snapped coordinates
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

                // Add to chunk instead of global vector
                AddTile(tile, x * tileSize, y * tileSize, tileSize);
            }
        }
    }
}

void World::Draw(Camera2D camera, int screenW, int screenH, int tileSize) {
    // Frustum Culling: Find visible chunk range
    float viewLeft = camera.target.x - (camera.offset.x / camera.zoom);
    float viewTop = camera.target.y - (camera.offset.y / camera.zoom);
    float viewRight = viewLeft + (screenW / camera.zoom);
    float viewBottom = viewTop + (screenH / camera.zoom);

    int chunkSizePixels = CHUNK_SIZE * tileSize;
    
    int startCX = floor(viewLeft / chunkSizePixels);
    int endCX = floor(viewRight / chunkSizePixels);
    int startCY = floor(viewTop / chunkSizePixels);
    int endCY = floor(viewBottom / chunkSizePixels);

    // Only loop through visible chunks
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
    int chunkSizePixels = CHUNK_SIZE * tileSize;
    
    // 1. Calculate the BOUNDARIES in chunk coordinates
    // We use floor to ensure we get the correct index even for negative coordinates
    int startCX = (int)floor((center.x - radius) / chunkSizePixels);
    int endCX   = (int)floor((center.x + radius) / chunkSizePixels);
    int startCY = (int)floor((center.y - radius) / chunkSizePixels);
    int endCY   = (int)floor((center.y + radius) / chunkSizePixels);

    // DEBUG: Uncomment this to see how many chunks you are checking in your console
    // printf("Checking Chunks from X:%d to %d, Y:%d to %d\n", startCX, endCX, startCY, endCY);

    // 2. Loop through every chunk in that square range
    for (int cx = startCX; cx <= endCX; cx++) {
        for (int cy = startCY; cy <= endCY; cy++) {
            
            std::pair<int, int> chunkKey = {cx, cy};

            // 3. Only proceed if this chunk actually exists in our map
            if (chunks.find(chunkKey) != chunks.end()) {
                auto& v = chunks[chunkKey].tiles;
                
                // 4. Standard iterator-based removal
                for (auto it = v.begin(); it != v.end(); ) {
                    Vector2 tilePos = { it->getPos().x + (tileSize / 2.0f), it->getPos().y + (tileSize / 2.0f) };
                    
                    // Use Raylib's built-in circle-point check
                    if (CheckCollisionPointCircle(tilePos, center, radius)) {
                        it = v.erase(it); 
                    } else {
                        ++it;
                    }
                }
            }
        }
    }
}