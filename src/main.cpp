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
    textureAssets.Load("STONEWALL", "resources/StoneWall.png");
    textureAssets.Load("COBBLESTONE", "resources/CobbleStone.png");
    textureAssets.Load("SNOW", "resources/Snow.png");
    textureAssets.Load("PLAYER", "resources/Player.png");
    textureAssets.Load("AMBERORE", "resources/AmberOre.png");
    textureAssets.Load("LOG", "resources/Log.png");
    textureAssets.Load("LEAVES", "resources/Leaves.png");
    textureAssets.Load("CRAFTER", "resources/Crafter.png");
    textureAssets.Load("COPPARORE", "resources/CopparOre.png");
    textureAssets.Load("IRONORE", "resources/IronOre.png");
    textureAssets.Load("TNT", "resources/TNT.png");

}


int main(){
    
    const int winW = 1080;
    const int winH = 720;
    bool debugMode = true;
    std::string gameName = "Cosmic Cavity";
    std::string gameVersion = "Beta 1";
    
    int FPS = 60;

    
    InitWindow(winW, winH, gameName.c_str());
    SetTargetFPS(FPS);
    LoadTextures();
    
    

    std::vector<Tile*> worldTiles;

    int tileSize = 120;
    int worldW = 1500;
    int worldH = 500;

    World gameWorld;

    gameWorld.Generate(tileSize, worldW, worldH);

    
    
    Player player(winW, winH);
    TileDef crafterDef = {"CRAFTER","CRAFTER", "CRAFTER", 999, false, TileShape::FULL_BLOCK};
    TileDef stoneWallDef = {"STONEWALL","STONEWALL", "STONEWALL", 999, false, TileShape::FULL_BLOCK};

    TileDef explosiveDef = {"TNT","TNT", "TNT", 999, true, TileShape::FULL_BLOCK, true, 800.0f};

    player.getInventory().AddItem(new BlockItem("CRAFTER", crafterDef, 999));
    player.getInventory().AddItem(new BlockItem("STONEWALL", stoneWallDef, 999));
    player.getInventory().AddItem(new ExplosiveBlock("TNT", explosiveDef, 999));


    while(!WindowShouldClose()){
        float dt = GetFrameTime();
        
        player.Update(gameWorld, tileSize);

        

        BeginDrawing();
        if(player.GetRect().y < (40*tileSize)){
            ClearBackground(SKYBLUE);
        }else{
            ClearBackground(DARKBROWN);
        }
        
        BeginMode2D(player.getCamera());
        gameWorld.Draw(player.getCamera(), winW, winH, tileSize);

        player.DrawHighlights(tileSize);
        gameWorld.DrawItems();
        gameWorld.UpdateItems(player, dt, tileSize);
        player.Draw();
        EndMode2D();

        
        
        player.getInventory().DrawInventory(winW, winH);
        if(debugMode){
            DrawText(TextFormat("%s : %s",gameName.c_str(), gameVersion.c_str()), 10, 10, 20, WHITE);
            DrawText(TextFormat("FPS: %d", GetFPS()), 10, 50, 20, WHITE);
            DrawText(TextFormat("Delta Time: %.4f ms", GetFrameTime()), 10, 70, 20, WHITE);
            DrawText(TextFormat("X, Y Pos: %.0f, %.0f", player.GetRect().x / tileSize, player.GetRect().y / tileSize), 10, 110, 20, WHITE);
            
        }
        
        EndDrawing();

    }   
    CloseWindow();

}