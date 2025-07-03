// src/enemy.h
#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "types.h"         // Contém a definição da struct Enemy, Player e enums
#include "asset_manager.h" // Para GameAssets

// Declarações de funções do inimigo
void InitEnemy(Enemy* enemy);
void UpdateEnemy(Enemy* enemy, float dt, const Player* player);
void DrawEnemy(const Enemy* enemy, const GameAssets* assets);

// Funções auxiliares
Rectangle GetEnemyHitbox(const Enemy* enemy);
int EnemyTakeDamage(Enemy* enemy, int damage);

#endif // ENEMY_H