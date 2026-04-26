#pragma once
#include <raylib.h>
#include <vector>
#include <string>
#include <map>
#include "Tiles.h"

// --- NEW: Forward Declarations to stop the loops ---
class ItemDrop; 
class Player;
// --------------------------------------------------

const int CHUNK_SIZE = 16;

struct Chunk {
    std::vector<Tile> tiles;
};

class World {
public:
    World(); 
    ~World();

    std::map<std::pair<int, int>, Chunk> chunks;
    std::vector<ItemDrop*> droppedItems;


    Tile CreateTileFromBlueprint(TileDef def, int x, int y, int tileSize);
    void Generate(int tileSize, int worldW, int worldH);
    void Draw(Camera2D camera, int screenW, int screenH, int tileSize);
    void Explode(Vector2 center, float radius, int tileSize);

    std::pair<int, int> GetChunkCoords(int worldX, int worldY, int tileSize);
    void AddTile(TileDef def, int x, int y, int tileSize);
    void RemoveTileAt(int worldX, int worldY, int tileSize);

    void SpawnPhysicalDrop(Item* data, float x, float y, int velX, int velY);
    void AddDroppedItem(ItemDrop* item);
    void UpdateItems(Player& player, float dt, int tileSize);
    void DrawItems();

private:
    float Noise1D(float x, float seed);
    float Noise2D(float x, float y, float seed);
};