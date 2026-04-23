#include <raylib.h>
#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <ctime>

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
    
    Rectangle getRec() const {return rec;}
    Vector2 getPos() const {return {x, y};}
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


class Player{
    
    Vector2 mousePos;
    Camera2D camera;
    float movementSpeed = 10.0f;
    
    public:
    Player(){
        camera = { 0 };
        camera.target = {0, 0};
        camera.offset = {1080/2.0f, 720/2.0f};
        camera.rotation = 0.0f;
        camera.zoom = 1.0f;
    }

    Vector2 GetMouseMouse(){
        return GetScreenToWorld2D(GetMousePosition(), camera);
    }


    void Update(std::vector<Tile>& worldTiles, int tileSize){

        //Camera
        if(IsKeyDown(KEY_D)) camera.target.x += movementSpeed;
        if(IsKeyDown(KEY_A)) camera.target.x -= movementSpeed;
        if(IsKeyDown(KEY_W)) camera.target.y -= movementSpeed;
        if(IsKeyDown(KEY_S)) camera.target.y += movementSpeed;


        //Tile Placement
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){

            mousePos = GetMouseMouse();
            int snappedX = (int)(mousePos.x/tileSize) * tileSize;
            int snappedY = (int)(mousePos.y/tileSize) * tileSize;
            
            bool isOcupied = false;

            for (const auto& tile : worldTiles){
                if(tile.getPos().x == snappedX && tile.getPos().y == snappedY){
                    isOcupied = true;
                    break;
                }
            }

            if(!isOcupied){
                worldTiles.push_back(Tile(snappedX, snappedY, tileSize, tileSize, "GRASS"));
            }

            
            
        }
        //Remove Tile
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            mousePos = GetMouseMouse();
            for (int i = 0; i < worldTiles.size(); i++){
                if(CheckCollisionPointRec(mousePos, worldTiles[i].getRec())){
                    worldTiles.erase(worldTiles.begin()+i);
                    break;
                }
            }
                    
        }


    }
    

    Camera2D getCamera() {return camera;}

};






int main(){
    
    int winW = 1080;
    int winH = 720;

    

    
    int FPS = 60;

    InitWindow(winW, winH, "Terraria");
    SetTargetFPS(FPS);
    LoadTextures();


    std::vector<Tile> worldTiles = {};

    int tileSize = 120;
    int worldW = tileSize*100;
    int worldH = tileSize*100;


    
    Player player;
    
    float seed = 1234.5;
    

    for (int x = 0; x < 10; x++){
            
        float bigHills = sin((x+seed) * 0.1f) * 10.0f;
        float smallHills = sin((x+seed) * 0.4f) * 2.0f;

        int surfaceY = 0 + sin((x+seed) * 0.2f) * 3.0f;

        for (int y = surfaceY; y < worldH; y++){
            Tile tile(x*tileSize, y*tileSize, tileSize, tileSize, "GRASS");
            worldTiles.push_back(tile);
        }

    }





    while(!WindowShouldClose()){

        
        player.Update(worldTiles, tileSize);
        BeginDrawing();
        ClearBackground(SKYBLUE);

        BeginMode2D(player.getCamera());
        

        

        for(int id = 0; id < worldTiles.size(); id++){
            worldTiles[id].DrawTile();
        }
        
        EndMode2D();

        EndDrawing();

    }   
    CloseWindow();

}