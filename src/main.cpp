#include <raylib.h>
#include <iostream>
#include <vector>
#include <map>
#include <random>

class TextureHandler{
    public:
    std::map<std::string, Texture2D> textures = {};
    void Load(std::string ID, std::string Path){
       if(textures.find(ID) == textures.end()){
        Texture2D texture = LoadTexture(Path.c_str());
        textures[ID] = texture;
       }
    }

    Texture2D Get(std::string ID){
        Texture2D tex = textures.at(ID);
        return tex;
    }
};

TextureHandler textures;

void LoadTextures(){
    textures.Load("DEBUG", "resources/CatDebug.jpg");
    textures.Load("GRASS", "resources/Grass.png");
    textures.Load("DIRT", "resources/CatDebug.jpg");
    textures.Load("STONE", "resources/CatDebug.jpg");
    textures.Load("GOLD", "resources/CatDebug.jpg");
}

void DrawTextureFromTextures(Texture2D texture, Rectangle rec){
    
    Rectangle sourceRec = {0.0f, 0.0f, (float)texture.width, (float)texture.height};


    DrawTexturePro(texture, sourceRec, rec, {0,0}, 0.0f, WHITE);
}


class TextureBlock{
    int x;
    int y;
    int w;
    int h;
    Rectangle rec;
    std::string ID;
    public:
    TextureBlock(int posX, int posY, int W, int H, std::string textureID){
        x = posX;
        y = posY;
        w = W;
        h = H;
        rec = {(float)x, (float)y, (float)w, (float)h};
        ID = textureID;
    }

    void DrawTile(){
        DrawTextureFromTextures(textures.Get(ID), rec);
    }

};

class Tile : public TextureBlock{
    public:
    Tile(int posX, int posY, int W, int H, std::string textureID) : TextureBlock(posX, posY, W, H, textureID){}


};






int main(){
    
    int winW = 1080;
    int winH = 720;
    
    int FPS = 60;

    InitWindow(winW, winH, "Terraria");
    
    LoadTextures();

    std::vector<Tile> WorldTiles = {};
    


    
    for (int y = 0; y < 10; y++){
        for (int x = 0; x < 10; x++){
            Tile tile(x*100, y*100, 100, 100, "GRASS");
            WorldTiles.push_back(tile);
        }
    }





    while(!WindowShouldClose()){

        BeginDrawing();

        for(int id = 0; id < WorldTiles.size(); id++){
            WorldTiles[id].DrawTile();
        }
        

        EndDrawing();

    }   
    CloseWindow();

}