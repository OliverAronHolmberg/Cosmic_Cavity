#include "World.h"
#include <cmath>

World::World() {

}

World::~World() {
    
    tiles.clear();
}


Tile World::GetTileAt(int worldX, int worldY) {
    for (Tile &t : tiles) {
        if (CheckCollisionPointRec({(float)worldX, (float)worldY}, t.getRec())) {
            return t;
        }
    }


    return Tile(0, 0, 0, 0, {"NONE", "NONE", "NONE", 0, false, TileShape::WALL});
}


void World::RemoveTileAt(int index) {
    if (index >= 0 && index < (int)tiles.size()) {
        tiles.erase(tiles.begin() + index);
    }
}


void World::CreateStructure(int startX, int startY, int tileSize) {

}

void World::Generate(int tileSize, int worldW, int worldH) {
    float seed = (float)GetRandomValue(1, 10000);
    
    tiles.reserve(worldW * 60);

    for (int x = 0; x < worldW; x++) {
   
        float mountains = Noise1D(x * 0.05f, seed) * 30.0f;
        float jagged = Noise1D(x * 0.2f, seed + 100) * 5.0f;
        int surfaceY = 15 + (int)(mountains + jagged);

        for (int y = surfaceY; y < worldH; y++) {
            float depth = (float)(y - surfaceY) / (float)(worldH - surfaceY);
            
       
            float caveShape = Noise2D(x * 0.05f, y * 0.05f, seed + 500);
        
            float caveThreshold = 0.1f + (depth * 0.1f);
            bool isCave = fabs(caveShape) < caveThreshold;

            if (!isCave) {
                TileDef tile;
                std::string name = "STONE";
                std::string drop = "COBBLESTONE";
                int dropAmount = 1;
                bool isPlaceable = true;
                bool hasCollision = true;
                if (surfaceY < 5) {
                    tile = {"SNOW", "SNOW", "SNOW", 1, true, TileShape::FULL_BLOCK};
                }
                else if (y == surfaceY) {
                    tile = {"GRASS", "GRASS", "GRASS", 1, true, TileShape::FULL_BLOCK};
                }
                else if (y < surfaceY + 4) {
                    tile = {"DIRT", "DIRT", "DIRT", 1, true, TileShape::FULL_BLOCK};
                }
                else {
                    float oreNoise = Noise2D(x * 0.6f, y * 0.6f, seed + 999);
                    float stoneNoise = Noise2D(x * 0.5f, y * 0.5f, seed + 777);

                    if (y > 100 && oreNoise > 0.8f) {
                        tile = {"AMBERORE", "AMBERORE", "AMBERORE", 1, true, TileShape::FULL_BLOCK};
                    }
                    else if (y > 30 && oreNoise > 0.88f){
                        tile = {"COPPARORE", "COPPARORE", "COPPARORE", 1, true, TileShape::FULL_BLOCK};
                    }
                    else if (y > 60 && oreNoise > 0.75f){
                        tile = {"IRONORE", "IRONORE", "IRONORE", 1, true, TileShape::FULL_BLOCK};
                    }
                    else if (stoneNoise > 0.85f) {
                        tile = {"COBBLESTONE", "COBBLESTONE", "COBBLESTONE", 1, true, TileShape::FULL_BLOCK};
                    } 
                    else {
                        tile = {"STONE", "STONE", "COBBLESTONE", 1, false, TileShape::FULL_BLOCK};
                    }
                }

                tiles.emplace_back(x * tileSize, y * tileSize, tileSize, tileSize, tile);
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
            t.Draw();
        }
    }
}


