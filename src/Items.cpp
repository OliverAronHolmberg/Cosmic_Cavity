#include "Items.h"
#include "World.h" 

void BlockItem::OnUse(World& world, int x, int y) {

}

void ExplosiveBlock::OnUse(World& world, int x, int y, int tileSize) {
    world.Explode({(float)x, (float)y}, this->blastRadius, tileSize);
}

void ToolItem::OnUse(World& world, int x, int y) {
    
}