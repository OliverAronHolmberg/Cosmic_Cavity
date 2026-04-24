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
    textures.Load("DIRT", "resources/Dirt.png");
    textures.Load("STONE", "resources/Stone.png");
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

class Inventory {

    int x;
    int y;
    int w;
    int h;


    public:
    Inventory(int X, int Y, int W, int H){

    }


};


class Player{
    
    Vector2 mousePos;
    Camera2D camera;
    float movementSpeed;
    public:
    Player(){
        camera = { 0 };
        camera.target = {0, 0};
        camera.offset = {1080/2.0f, 720/2.0f};
        camera.rotation = 0.0f;
        camera.zoom = 0.1f;
        movementSpeed = 10.0f/camera.zoom;
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

        camera.zoom += GetMouseWheelMove() * 0.1f;
        if(camera.zoom < 0.1f) camera.zoom = 0.1f;
        if(camera.zoom > 3.0f) camera.zoom = 3.0f;
        movementSpeed = 10.0f/camera.zoom;


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


float Noise1D(float x, float seed){

    auto hash = [](int x) {
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = (x >> 16) ^ x;
        return (float)x / 2147483647.0f;
    };
    int x0 = (int)floor(x);
    int x1 = x0 + 1;
    float t = x -(float)x0;
    float fade = t * t * (3-2*t);
    return hash(x0 + (int)seed) * (1.0f - fade) + hash(x1 + (int)seed) * fade;

}

float Noise2D(float x, float y, float seed){
    auto hash = [](int x, int y, int s){
        unsigned int h = (unsigned int)s  ^(unsigned int)x * 1327217883 ^(unsigned int)y * 374761393;
        h = (h ^(h>>16)) * 0x85ebca6b;
        h = (h ^(h>>13)) * 0x85ebca6b;
        return (float)(h & 0x7FFFFFFF) / 2147483647.0f;
    };
    int ix = (int)floor(x);
    int iy = (int)floor(y);
    float fx = x -(float)ix;
    float fy = y -(float)iy;

    float ux = fx*fx*(3.0f-2.0f * fx);
    float uy = fy*fy*(3.0f-2.0f * fy);

    float a = hash(ix, iy, (int)seed);
    float b = hash(ix + 1, iy, (int)seed);
    float c = hash(ix, iy+1, (int)seed);
    float d = hash(ix + 1, iy + 1, (int)seed);

    float x1 = a + ux *(b - a);
    float x2 = c + ux *(d - c);
    return x1 + uy * (x2 - x1);
}



int main(){
    
    int winW = 1080;
    int winH = 720;

    

    
    int FPS = 60;

    InitWindow(winW, winH, "Terraria");
    SetTargetFPS(FPS);
    LoadTextures();


    std::vector<Tile> worldTiles = {};

    int tileSize = 120;
    int worldW = 1500;
    int worldH = 500;

    
    
    Player player;
    
    float seed = GetRandomValue(1, 10000);
    float caveSize = 0.1f;
    
    worldTiles.reserve(worldW * worldH);

    
     for (int x = 0; x < worldW; x++){

        float mountians = Noise1D(x * 0.05f, seed) * 30.0f;
        float jagged = Noise1D(x * 0.2f, seed + 100) * 5.0f;

        int surfaceY = 10 + (int)(mountians + jagged);


        for (int y = surfaceY; y < worldH; y++){

            float caveValue = Noise2D(x * caveSize, y * caveSize, seed + 500);

            if(caveValue > 0.3f){
                std::string tex = (y == surfaceY) ? "GRASS" : "STONE";
                worldTiles.push_back(Tile(x*tileSize, y*tileSize, tileSize, tileSize, tex));
            }

            

        }



    }






    while(!WindowShouldClose()){

        
        player.Update(worldTiles, tileSize);
        BeginDrawing();
        ClearBackground(SKYBLUE);

        BeginMode2D(player.getCamera());
        
        Camera2D cam = player.getCamera();

        float viewLeft = cam.target.x - (cam.offset.x / cam.zoom);
        float viewTop = cam.target.y - (cam.offset.y / cam.zoom);
        float viewRight = viewLeft + (winW / cam.zoom);
        float viewBottom = viewTop + (winH / cam.zoom);

        int margin = tileSize*2;

        for(int id = 0; id < worldTiles.size(); id++){
            Rectangle tileRec = worldTiles[id].getRec();
            if (tileRec.x + tileRec.width > viewLeft - margin &&
                tileRec.x < viewRight + margin &&
                tileRec.y + tileRec.height > viewTop - margin &&
                tileRec.y < viewBottom + margin)
                {
                    worldTiles[id].DrawTile();
                }

            
        }
        
        EndMode2D();
        DrawFPS(10, 10);
        EndDrawing();

    }   
    CloseWindow();

}