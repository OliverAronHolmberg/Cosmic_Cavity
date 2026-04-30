#include "ItemDrop.h"
#include "TextureHandler.h"

ItemDrop::ItemDrop(Item* item, float x, float y, int velX, int velY) 
    : Entity(x, y, 40.0f, 40.0f, 0.0f) 
{
    velocity.x = velX;
    velocity.y = velY;
    this->itemData = item;

}

ItemDrop::~ItemDrop() {
    if (itemData != nullptr) {
        delete itemData;
    }
}

void ItemDrop::Update(World& world, int tileSize, float dt) {
    age += dt;
    if(isGrounded) velocity.x *= 0.9;
    ApplyPhysics(world, tileSize, dt);
}

void ItemDrop::Draw() { 
    if (!itemData) return;

   
    if (age > maxAge - 10.0f) {
        if ((int)(age * 10) % 2 == 0) return; 
    }

    Texture2D sprite = textureAssets.Get(itemData->name);
    Rectangle dest = { rect.x, rect.y, rect.width, rect.height };
    DrawTextureScaled(sprite, dest);
}