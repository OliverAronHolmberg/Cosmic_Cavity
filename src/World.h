#pragma once
#include <raylib.h>
#include <vector>
#include <string>
#include "Tiles.h"


class World{
    public:
    std::vector<Tile*> tiles;

    Tile* CreateTileFromID(std::string blockID, int x, int y, int tileSize);

    void Generate(int tileSizem, int worldW, int worldH);
    void Draw(Camera2D camera, int screenW, int screenH, int tileSize);

    Tile* GetTileAt(int worldX, int worldY);
    void RemoveTileAt(int index);

    ~World();

    private:
    float Noise1D(float x, float seed);
    float Noise2D(float x, float y, float seed);
    void CreateStructure(int startX, int startY, int tileSize);
};
