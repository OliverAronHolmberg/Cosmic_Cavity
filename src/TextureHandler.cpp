#include "TextureHandler.h"
#include <iostream>



TextureHandler textureAssets;


void TextureHandler::Load(std::string ID, std::string Path){
    if(textures.find(ID) == textures.end()){
        Texture2D texture = LoadTexture(Path.c_str());
        if(texture.id == 0) {
            std::cout << "Error Loading Texture At" << Path << std::endl;
        }else{
            textures[ID] = texture;
        }
        
       }
}

Texture2D TextureHandler::Get(std::string ID){
    if (ID == "NONE" || ID.empty()) return { 0 };
        auto it = textures.find(ID);
        if (it == textures.end()){
            auto debugIt = textures.find("DEBUG");
            if (debugIt != textures.end()) {
                return debugIt->second;
            }
            // Return a blank texture if nothing found
            return { 0 };
        }
        return it->second;
}

void TextureHandler::UnloadAll(){
    for (auto const& [id, tex] : textures){
        UnloadTexture(tex);
    }
    textures.clear();
}

void DrawTextureScaled(Texture2D texture, Rectangle destRec){
    if (texture.id == 0 || texture.width == 0 || texture.height == 0) return;
    Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height};
    DrawTexturePro(texture, sourceRec, destRec, {0,0}, 0.0f, WHITE);
}