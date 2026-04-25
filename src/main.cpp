#include <raylib.h>
#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <ctime>
#include <cctype>

int winW = 1080;
int winH = 720;

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
        if (ID == "NONE") return { 0 };
        auto it = textures.find(ID);

        if (it == textures.end()){
            return textures.at("DEBUG");
        }
        
        return it->second;
    }
};

TextureHandler textures;

void LoadTextures(){
    textures.Load("INVENTORYSLOT", "resources/InventoryTile.png");
    textures.Load("DEBUG", "resources/CatDebug.jpg");
    textures.Load("GRASS", "resources/Grass.png");
    textures.Load("DIRT", "resources/Dirt.png");
    textures.Load("STONE", "resources/Stone.png");
    textures.Load("COBBLESTONE", "resources/CobbleStone.png");
    textures.Load("SNOW", "resources/Snow.png");
    textures.Load("PLAYER", "resources/Player.png");
    textures.Load("AMBERORE", "resources/AmberOre.png");
    textures.Load("LOG", "resources/Log.png");
    textures.Load("LEAVES", "resources/Leaves.png");
    
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
    
    public:
    std::string ID;
    
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
    std::string dropID;
    int dropAmount;
    std::string tileName;

    Tile(int posX, int posY, int W, int H, std::string textureID, std::string Name, std::string drop, int amount = 1) : TextureBlock(posX, posY, W, H, textureID){
        tileName = Name;
        dropAmount = amount;
        dropID = drop;

    }


};

class itemUI : public TextureBlock{

    int x;
    int y;
    int w;
    int h;
    

    public:
    itemUI(int X, int Y, int W, int H, std::string ID) : TextureBlock(X, Y, W, H, ID){
        x = X;
        y = Y;
        w = W;
        h = H;
    }



};






class Item {
    public:
    std::string name;
    std::string textureID;
    int count;
    int max;


    Item(std::string Name, std::string texID, int amount = 1, int maxAmount = 999)
    : name(Name), textureID(texID), count(amount), max(maxAmount) {}

    virtual ~Item() = default;

    virtual void OnUse(){
        
    }
    

};

class BlockItem : public Item{

    public:
    std::string placeTileID;

    BlockItem(std::string Name, std::string TexID, int amount = 1) 
    : Item(Name, TexID, count, 999), placeTileID(TexID){}

    void OnUse() override {

    }

};

class ToolItem : public Item{
    public:
    ToolItem(std::string Name, std::string TexID) : Item(Name, TexID, 1, 1) {}

    void OnUse() override {

    }
};



struct TileOffset{
    int dx;
    int dy;
    std::string name;
};

struct Structure{
    std::vector<TileOffset> tiles;
};

Structure CreateTree(){
    Structure tree;
    tree.tiles.push_back({0, 0, "LOG"});
    tree.tiles.push_back({0, -1, "LOG"});
    tree.tiles.push_back({0, -2, "LOG"});
    tree.tiles.push_back({0, -3, "LOG"});

    tree.tiles.push_back({0, -4, "LEAVES"});
    tree.tiles.push_back({-1, -4, "LEAVES"});
    tree.tiles.push_back({-2, -4, "LEAVES"});
    tree.tiles.push_back({1, -4, "LEAVES"});
    tree.tiles.push_back({2, -4, "LEAVES"});
    
    tree.tiles.push_back({0, -5, "LEAVES"});
    tree.tiles.push_back({-1, -5, "LEAVES"});
    tree.tiles.push_back({-2, -5, "LEAVES"});
    tree.tiles.push_back({1, -5, "LEAVES"});
    tree.tiles.push_back({2, -5, "LEAVES"});

    tree.tiles.push_back({0, -6, "LEAVES"});
    tree.tiles.push_back({-1, -6, "LEAVES"});
    tree.tiles.push_back({1, -6, "LEAVES"});


    

    return tree;
    
}

Structure CreateBush(){
    Structure bush;

    bush.tiles.push_back({0, 0, "LEAVES"});
    bush.tiles.push_back({1, 0, "LEAVES"});
    bush.tiles.push_back({-1, 0, "LEAVES"});
    bush.tiles.push_back({0, 1, "LEAVES"});
    bush.tiles.push_back({1, 1, "LEAVES"});
    


    
}

class InventorySlot : public itemUI{
    Item* heldItem = nullptr;
    bool isOccupied = false;
    int x;
    int y;
    int w;
    int h;

    public:
    InventorySlot(int X, int Y, int W, int H) : itemUI(X, Y, W, H, "INVENTORYSLOT"){
        x = X;
        y = Y;
        w = W;
        h = H;
    }
    
    void SetItem(Item* newItem){
        heldItem = newItem;
        isOccupied = (heldItem != nullptr);
    }

    bool getOccupied() {return isOccupied;}
    std::string getItemName() {return heldItem->name;}
    std::string getItemID() {
        if(!isOccupied || heldItem->count <= 0) return "NONE";
        return heldItem->textureID;
    }

    void incementItemCount(int amount){
        heldItem->count += amount;
        if(heldItem->count <= 0){
            
            isOccupied = false;
            heldItem->count = 0;
            heldItem->textureID = "NONE";
            heldItem->name = "Empty";
        }else{
            isOccupied = true;
        }
    }

    void DrawSlot(){
        this->DrawTile();

       
        if(isOccupied && heldItem){
            Rectangle itemRec = { (float)x + 5, (float)y + 5, (float)w - 10, (float)h - 10};
            DrawTextureFromTextures(textures.Get(heldItem->textureID), itemRec);
            DrawText(TextFormat("%i", heldItem->count), x + 5, y + 5, 20, WHITE);
        }
        

        

    }


    ~InventorySlot() {delete heldItem;}
};

class Inventory{
    std::vector<InventorySlot> slots;
    int rows = 5;
    int cols = 9;
    int x;
    int y;
    int w;
    int h;
    int selectedSlot = 0;

    public:
    bool isOpened = false;
    int GetInventoryToggle() {return isOpened;}
    void ToggleInventory() {isOpened = !isOpened;}
    void setSelectedSlot(int slotNumber){selectedSlot = slotNumber - 1;}
    std::string getItemID() {
        return slots[selectedSlot].getItemID();
    }
    std::string getSelectedItemName(){
        return slots[selectedSlot].getItemName();
    }
    

    Inventory(int X, int Y, int W, int H){
        x = X;
        y = Y;
        h = H;
        w = W;

        int slotSize = 75;
        int padding = 0;

        int totalGridWidth = (cols * slotSize) + ((cols -1) * padding);
        int centeredOffset = (W - totalGridWidth) / 2;

        for (int r = 0; r < rows; r++){
            for (int c = 0; c < cols; c++){
                int slotX = X + centeredOffset + (c* (slotSize + padding));
                int slotY = Y - (r * (slotSize + padding));
                slots.push_back(InventorySlot(slotX, slotY, slotSize, slotSize));
            }
        }


    }


    void DrawInventory(){
        if(isOpened){
            DrawRectangle(0,0, winW, winH, ColorAlpha(DARKGRAY, 0.75f));
            for (auto& slot : slots){
                slot.DrawSlot();
            }

        }else{
            for (int i = 0; i < cols; i++){
                slots[i].DrawSlot();
                highlightSelectedSlot();
            }
        }
    }

    void removeItemFromSelected(int amount){
        slots[selectedSlot].incementItemCount(-amount);
    }
    bool AddItem(Item* newItem){


        // Add Existing Item
        for(auto& slot : slots){
            if (slot.getOccupied() && slot.getItemName() == newItem->name){
                slot.incementItemCount(newItem->count);
                return true;
            }
        }

        // Find Empty slot
        for (auto& slot : slots){
            if (!slot.getOccupied()){
                slot.SetItem(newItem);
                return true;
            }
        }

        return false;

    }

    void highlightSelectedSlot(){
        for (int i = 0; i < cols; i++){
                if(i == selectedSlot){
                   Rectangle highlightSlotRec = slots[i].getRec();
                   DrawRectangleLinesEx(highlightSlotRec, 5.0f, WHITE);
                }
            }
    }

};





class Player{
    int slotSize = 75;
    int invMargin = 10;
    Vector2 mousePos;
    Camera2D camera;
    float movementSpeed;

    int snappedX;
    int snappedY;

    Inventory inventory;

    Texture2D invTex = textures.Get("INVENTORYSLOT");

    bool Isflying = false;
    float accelerationY = 0.0f;
    const float gravity = 0.5f;
    bool isGrounded = false;
    float jumpHeight = 20.0f;

    public:
    Rectangle playerRec = {100, 100, 75, 175};

    Player(int winW, int winH) : inventory((winW-winW/2)/2, winH-slotSize-invMargin, winW/2, slotSize){
        camera = { 0 };
        camera.target = {0, 0};
        camera.offset = {1080/2.0f, 720/2.0f};
        camera.rotation = 0.0f;
        camera.zoom = 1.0f;
        movementSpeed = 10.0f/camera.zoom;

    }

    Inventory& getInventory() {return inventory;}

    Vector2 GetMouseMouse(){
        return GetScreenToWorld2D(GetMousePosition(), camera);
    }

    

    void Update(std::vector<Tile>& worldTiles, int tileSize){
    
        

        //Camera & movement
        float deltaX = 0;
        if(!inventory.isOpened){
            if(IsKeyDown(KEY_D)) deltaX += movementSpeed;
            if(IsKeyDown(KEY_A)) deltaX -= movementSpeed;
        }
        

        playerRec.x += deltaX;

        for (const auto& tile : worldTiles){
            if(CheckCollisionRecs(playerRec, tile.getRec())){
                if(deltaX > 0) playerRec.x = tile.getRec().x - playerRec.width;
                if(deltaX < 0) playerRec.x = tile.getRec().x + tile.getRec().width;
            }
        }

        if(!Isflying){
            accelerationY += gravity;
        }else{
            if(!inventory.isOpened){
                if(IsKeyDown(KEY_W)) playerRec.y -= movementSpeed;
                if(IsKeyDown(KEY_S)) playerRec.y += movementSpeed;

            }
        }

        playerRec.y += accelerationY;

       
        isGrounded = false;
        for (const auto& tile : worldTiles){
            if(CheckCollisionRecs(playerRec, tile.getRec())){
                if(accelerationY > 0){
                    playerRec.y = tile.getRec().y - playerRec.height;
                    accelerationY = 0;
                    isGrounded = true;
                }
                else if(accelerationY < 0){
                playerRec.y  = tile.getRec().y + tile.getRec().height;
                accelerationY = 0;
            }
            }
        }
        if((IsKeyDown(KEY_W) && isGrounded && !Isflying) || (IsKeyDown(KEY_SPACE) && isGrounded && !Isflying)){
            accelerationY = -jumpHeight;
            isGrounded = false;
        }

        camera.target = {playerRec.x + playerRec.width/2, playerRec.y + playerRec.height/2};

        

        if(IsKeyPressed(KEY_TAB)){
            inventory.ToggleInventory();
        }

        if(IsKeyPressed(KEY_ONE)){inventory.setSelectedSlot(1);}
        if(IsKeyPressed(KEY_TWO)){inventory.setSelectedSlot(2);}
        if(IsKeyPressed(KEY_THREE)){inventory.setSelectedSlot(3);}
        if(IsKeyPressed(KEY_FOUR)){inventory.setSelectedSlot(4);}
        if(IsKeyPressed(KEY_FIVE)){inventory.setSelectedSlot(5);}
        if(IsKeyPressed(KEY_SIX)){inventory.setSelectedSlot(6);}
        if(IsKeyPressed(KEY_SEVEN)){inventory.setSelectedSlot(7);}
        if(IsKeyPressed(KEY_EIGHT)){inventory.setSelectedSlot(8);}
        if(IsKeyPressed(KEY_NINE)){inventory.setSelectedSlot(9);}


        // camera.zoom += GetMouseWheelMove() * 0.1f;
        // if(camera.zoom < 0.1f) camera.zoom = 0.1f;
        // if(camera.zoom > 3.0f) camera.zoom = 3.0f;
        // movementSpeed = 10.0f/camera.zoom;

        if(inventory.isOpened == false){
            mousePos = GetMouseMouse();
            snappedX = (int)(std::floor(mousePos.x/tileSize)) * tileSize;
            snappedY = (int)(std::floor(mousePos.y/tileSize)) * tileSize;
    
            //Tile Placement
            if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
                bool isOcupied = false;
    
                for (const auto& tile : worldTiles){
                    if(tile.getPos().x == snappedX && tile.getPos().y == snappedY){
                        isOcupied = true;
                        break;
                    }
                }
    
                Rectangle placementRec = {(float)snappedX, (float)snappedY, (float)tileSize, (float)tileSize};
    
                bool hitsPlayer = CheckCollisionRecs(playerRec, placementRec);
    
                if(!isOcupied && !hitsPlayer){
    
                    if(inventory.getItemID() != "NONE"){
                        std::string ID = inventory.getItemID();
                        std::string name = inventory.getSelectedItemName();
                        worldTiles.push_back(Tile(snappedX, snappedY, tileSize, tileSize, ID, name, ID, 1));
                        inventory.removeItemFromSelected(1);
                    }
                    
                }
    
                
                
            }
    
            //Remove Tile
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
    
    
                for (int i = 0; i < worldTiles.size(); i++){
                    if(CheckCollisionPointRec(mousePos, worldTiles[i].getRec())){
    
                        std::string dropID = worldTiles[i].dropID;
                        int dropAmount = worldTiles[i].dropAmount;
                        std::string dropName = worldTiles[i].tileName;

    
                        Item* droppedItem = new Item(dropID, dropID, dropAmount);
                        inventory.AddItem(droppedItem);
    
                        worldTiles.erase(worldTiles.begin()+i);
                        break;
                    }
                }
    
                
                        
            }

        }



    }

    void DrawHighlight(int tileSize){
        Rectangle hightlightRec = {(float)snappedX, (float)snappedY, (float)tileSize, (float)tileSize};
        DrawRectangleLinesEx(hightlightRec, 3.0f, WHITE);
    }

    void DrawPlayerSprite(){
        Texture2D sprite = textures.Get("PLAYER");

        Rectangle sourceRec = {0.0f, 0.0f, (float)sprite.width, (float)sprite.height};
        float drawW = playerRec.width+30;
        float drawH = playerRec.height+25;

        Rectangle destRec = {playerRec.x + (playerRec.width / 2) - (drawW / 2),
                            (playerRec.y + playerRec.height) -drawH,
                            drawW,
                            drawH
        };

        // DrawRectangleRec(playerRec, WHITE);
        DrawTexturePro(sprite, sourceRec, destRec, {0, 0}, 0.0f, WHITE);
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

bool isTileAt(int tx, int ty, const std::vector<Tile>& worldTiles, int tileSize){
        for(const auto& tile : worldTiles){
            if(tile.getPos().x == tx * tileSize && tile.getPos().y == ty*tileSize){
                return true;
            }
        }
        return false;
    }

void GenerateWorld(int tileSize, int worldW, int worldH, std::vector<Tile>& worldTiles){
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
                if(surfaceY < 5) {
                    name = "SNOW";
                    drop = "SNOW";
                    dropAmount = 1;
                }
                else if (y == surfaceY){
                    name = "GRASS";
                    drop = "GRASS";
                    dropAmount = 1;
                    float spawnNoise = Noise1D(x*0.5f, seed + 1234);

                    if(spawnNoise > 0.9f && name == "Grass"){
                        Structure tree = CreateTree();
                        for(auto& offset : tree.tiles){
                            int tx = x + offset.dx;
                            int ty = surfaceY + offset.dy;

                            if(!isTileAt(tx, ty, worldTiles, tileSize)){
                                std::string texID = offset.name;
                                for (auto & c : texID) c = toupper(c);
                                worldTiles.push_back(Tile(tx*tileSize, ty*tileSize, tileSize, tileSize, texID, texID, texID, dropAmount));
                            }

                            
                        }
                    }
                    

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
                        }
                    }
                }
                

                
                worldTiles.push_back(Tile(x*tileSize, y*tileSize, tileSize, tileSize, name, name, drop, dropAmount));
            }

            

        }
    }
}

class WorldObject {
    public:
        Rectangle rec;
        std::string ID;
        bool hasCollision;
        bool isInteractable;

        float drawWidth;
        float drawHeight;

        WorldObject(float x, float y, float w, float h, std::string id, bool col){
            rec = {x, y, w, h};
            drawWidth = w;
            drawHeight = h;
            ID = id;
            hasCollision = col;
        }

    virtual void Update()
    {
        
    } 
    virtual void Draw(){
        DrawTextureFromTextures(textures.Get(ID), rec);

    }
};

class Door : public WorldObject{
    public:
    bool isOpened = false;
    void Interact(){
        isOpened = !isOpened;
        hasCollision = isOpened;
        ID = isOpened ? "DOOR_OPEN" : "DOOR_CLOSED";
    }
};



int main(){
    
    


    
    int FPS = 60;

    InitWindow(winW, winH, "Cavity Cascade");
    SetTargetFPS(FPS);
    LoadTextures();
    
    

    std::vector<Tile> worldTiles = {};

    int tileSize = 120;
    int worldW = 1500;
    int worldH = 500;

    GenerateWorld(tileSize, worldW, worldH, worldTiles);

    
    
    Player player(winW, winH);



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
        
        player.DrawHighlight(tileSize);
        player.DrawPlayerSprite();
        EndMode2D();
        
        player.getInventory().DrawInventory();
        DrawFPS(10, 10);
        EndDrawing();

    }   
    CloseWindow();

}