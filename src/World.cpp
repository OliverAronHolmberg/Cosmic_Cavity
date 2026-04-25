#include "World.h"
#include <cmath>


World::~World() {
    for (Tile* t : tiles){
        delete t;
    }
    tiles.clear();
}

Tile* World::CreateTileFromID(std::string blockID, int x, int y, int tileSize){
    bool hasCollision = true;
    std::string dropItem = blockID;

    return new Tile(x, y, tileSize, tileSize, blockID, blockID, dropItem, 1, true, hasCollision);
}


void World::Generate(int tileSize, int worldW, int worldH){
    float seed = GetRandomValue(1, 10000);
    float caveSize = 0.05f;
    
    for (int x = 0; x < worldW; x++){
        float mountians = Noise1D(x * 0.05f, seed) * 30.0f;
        float jagged = Noise1D(x * 0.2f, seed + 100) * 5.0f;

        int surfaceY = 10 + (int)(mountians + jagged);

        for (int y = surfaceY; y < worldH; y++){


            float depth = (float)(y-surfaceY) / (float)(worldH-surfaceY);
            
            float caveShape = Noise2D(x * caveSize, y * caveSize, seed + 500);

            float caveThreashold = 0.02f + (depth * 0.12f);

            bool isCave = fabs(caveShape - 0.25f) < caveThreashold;
            
            if(!isCave){
                std::string name;
                std::string drop;
                int dropAmount = 1;
                bool isPlaceable = true;
                bool hasCollision = true;
                if(surfaceY < 5) {
                    name = "SNOW";
                    drop = "SNOW";
                    dropAmount = 1;
                }
                else if (y == surfaceY){
                    name = "GRASS";
                    drop = "GRASS";
                    dropAmount = 1;
                    isPlaceable = false;
                    

            
                }else if (y < surfaceY + 6){
                    name = "DIRT";
                    drop = "DIRT";
                    dropAmount = 1;
                }
                else{
                    float oreNoise = Noise2D(x * 0.4f, y*0.4f, seed + 999);
                    if (oreNoise < 0.1f && y > 100){
                        name = "AMBERORE";
                        drop = "AMBERORE";
                        dropAmount = 5;
                    }else{
                        float stoneNoise = Noise2D(x*0.5f, y*0.5f, seed + 777);
                        if(stoneNoise > 0.8f){
                            name = "COBBLESTONE";
                            drop = "COBBLESTONE";
                            dropAmount = 1;
                        }else{
                            name = "STONE";
                            drop = "COBBLESTONE";
                            dropAmount = 1;
                            hasCollision = true;
                        }
                    }
                }
               
                tiles.push_back(new Tile(x*tileSize, y*tileSize, tileSize, tileSize, name, name, drop, dropAmount, isPlaceable, hasCollision));
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

void World::Draw(Camera2D camera, int screenW, int screenH, int tileSize){
    float viewLeft = camera.target.x - (camera.offset.x / camera.zoom);
    float viewTop = camera.target.y - (camera.offset.y / camera.zoom);
    float viewRight = viewLeft + (screenW / camera.zoom);
    float viewBottom = viewTop + (screenH / camera.zoom);
    int margin = tileSize*2;

    for(Tile*t : tiles){
        Rectangle rec = t->getRec();
        if (rec.x + rec.width > viewLeft - margin &&
                rec.x < viewRight + margin &&
                rec.y + rec.height > viewTop - margin &&
                rec.y < viewBottom + margin)
                {
                   t->DrawTile();
                }
    }
}
