// src/game.h
#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "types.h"          // Contém todas as definições de structs e enums
#include "asset_manager.h"  // Para GameAssets

// Declarações de funções do jogo
void InitGame(Game* game, Vector2 playerStartPos);
void UpdateGame(Game* game, float dt);
void DrawGame(const Game* game, const GameAssets* assets);
void UnloadGame(Game* game);

// Funções auxiliares para o jogo
void SpawnEnemy(Game* game);
void LevelUpPlayer(Game* game);
void GenerateBuffOptions(Game* game);

// NOVO: Funções para o sistema de ranking
void LoadRanking(Game* game, const char* filename);
void SaveRanking(const Game* game, const char* filename);
void AddScoreToRanking(Game* game, const char* playerName, int level, float survivalTime);

#endif // GAME_H