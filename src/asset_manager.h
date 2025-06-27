#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "raylib.h"


#define TILE_SIZE 16 // 16X16

// Estrutura para armazenar todas as texturas do jogo
typedef struct {
    Texture2D playerIdleSheet;
    Texture2D playerRunSheet;
    Texture2D playerHitDeathSheet;
    Texture2D snakeFullSheet;
    Texture2D bombExplosionSheet;
    Texture2D infiniteMapSheet; 
} GameAssets;

// Declarações de funções
void LoadGameAssets(GameAssets* assets);
void UnloadGameAssets(GameAssets* assets);

#endif // ASSET_MANAGER_H