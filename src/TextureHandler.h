#pragma once
#include <raylib.h>
#include <map>
#include <string>


class TextureHandler{
    public:
    std::map<std::string, Texture2D> textures;

    void Load(std::string ID, std::string Path);

    Texture2D Get(std::string ID);

    void UnloadAll();

    
};

extern TextureHandler textureAssets;

void DrawTextureScaled(Texture2D texture, Rectangle destRec);