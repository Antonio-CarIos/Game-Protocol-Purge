// src/bomb.h
#ifndef BOMB_H
#define BOMB_H

#include "raylib.h"
#include "types.h"         // Contém a definição da struct Bomb e enums
#include "asset_manager.h" // Para GameAssets

// Declarações de funções da bomba
void InitBomb(Bomb* bomb);
void UpdateBomb(Bomb* bomb, float dt);
void DrawBomb(const Bomb* bomb, const GameAssets* assets);

#endif // BOMB_H