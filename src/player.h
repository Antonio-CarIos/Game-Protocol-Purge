// src/player.h
#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "types.h"         // Agora Player, BuffType são conhecidos
#include "asset_manager.h" // Para GameAssets

// Declarações de funções do player
void InitPlayer(Player* player, Vector2 startPos);
void UpdatePlayer(Player* player, float dt);
void DrawPlayer(const Player* player, const GameAssets* assets);

// Funções auxiliares
Rectangle GetPlayerHitbox(const Player* player);
void PlayerTakeDamage(Player* player, int damage);
void PlayerApplyBuff(Player* player, BuffType type, float value);

#endif // PLAYER_H