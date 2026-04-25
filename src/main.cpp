#include <raylib.h>
#include "constants.h"
#include "TextureHandler.h"
#include "World.h"
#include "Player.h"





void LoadTextures(){
    textureAssets.Load("INVENTORYSLOT", "resources/InventoryTile.png");
    textureAssets.Load("DEBUG", "resources/CatDebug.jpg");
    textureAssets.Load("GRASS", "resources/Grass.png");
    textureAssets.Load("DIRT", "resources/Dirt.png");
    textureAssets.Load("STONE", "resources/Stone.png");
    textureAssets.Load("COBBLESTONE", "resources/CobbleStone.png");
    textureAssets.Load("SNOW", "resources/Snow.png");
    textureAssets.Load("PLAYER", "resources/Player.png");
    textureAssets.Load("AMBERORE", "resources/AmberOre.png");
    textureAssets.Load("LOG", "resources/Log.png");
    textureAssets.Load("LEAVES", "resources/Leaves.png");
    textureAssets.Load("CRAFTER", "resources/Crafter.png");

}


int main(){
    
    const int winW = 1080;
    const int winH = 720;

    
    int FPS = 60;

    InitWindow(winW, winH, "Cosmic Cavity");
    SetTargetFPS(FPS);
    LoadTextures();
    
    

    std::vector<Tile*> worldTiles;

    int tileSize = 120;
    int worldW = 1500;
    int worldH = 500;

    World gameWorld;

    gameWorld.Generate(tileSize, worldW, worldH);

    
    
    Player player(winW, winH);
    Item* crafter = new BlockItem("CRAFTER", "CRAFTER", 999, true, false);
    player.getInventory().AddItem(crafter);


    while(!WindowShouldClose()){

        
        player.Update(gameWorld, tileSize);
        BeginDrawing();
        ClearBackground(SKYBLUE);

        BeginMode2D(player.getCamera());
        gameWorld.Draw(player.getCamera(), winW, winH, tileSize);
        player.DrawHighlights(tileSize);
        player.Draw();
        EndMode2D();
        
        player.getInventory().DrawInventory(winW, winH);
        DrawFPS(10, 10);
        EndDrawing();

    }   
    CloseWindow();

}