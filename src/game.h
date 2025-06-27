#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "asset_manager.h"
#include "player.h"
#include "enemy.h" 
#include "bomb.h" 
#include "map.h"   

// Definir o número máximo de inimigos e bombas
#define MAX_ENEMIES 10
#define MAX_BOMBS 2

typedef struct {
    Player player;
    Enemy enemies[MAX_ENEMIES]; // Array de inimigos
    Bomb bombs[MAX_BOMBS];      // Array de bombas
    GameMap map;               

    float enemySpawnTimer; // Timer para spawnar inimigos
    float enemySpawnInterval; // Intervalo entre spawns de inimigos

} Game;

// Declarações de funções do jogo
void InitGame(Game* game, Vector2 playerStartPos);
void UpdateGame(Game* game, float dt);
void DrawGame(const Game* game, const GameAssets* assets);
void UnloadGame(Game* game); 

// Funções auxiliares para o jogo
void SpawnEnemy(Game* game, int screenWidth, int screenHeight);
void PlaceBombsNearPlayer(Game* game, const Player* player);


#endif // GAME_H