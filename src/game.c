#include "game.h"
#include "raylib.h"
#include "player.h"
#include "enemy.h"
#include "bomb.h"
#include "map.h"      // Para usar InitMap e DrawMap
#include <stdlib.h>   // Para rand()
#include <time.h>     
#include "raymath.h"  // Para Vector2Distance, Vector2Subtract, Vector2Scale


void InitGame(Game* game, Vector2 playerStartPos) {
    TraceLog(LOG_INFO, "GAME: Inicializando jogo...");
    InitPlayer(&game->player, playerStartPos);
    InitMap(&game->map); 

    // Inicializa todos os inimigos como inativos
    for (int i = 0; i < MAX_ENEMIES; i++) {
        InitEnemy(&game->enemies[i]);
    }
    game->enemySpawnTimer = 0.0f;
    game->enemySpawnInterval = 3.0f; // Spawnar um inimigo a cada 3 segundos

 
    for (int i = 0; i < MAX_BOMBS; i++) {
        InitBomb(&game->bombs[i]);
    }

   
    srand((unsigned int)time(NULL)); 
    TraceLog(LOG_INFO, "GAME: Jogo inicializado.");
}

void UpdateGame(Game* game, float dt) {
    // Atualiza o jogador
    UpdatePlayer(&game->player, dt);

    // --- Lógica de soltar Bombas (Pelo Jogador) ---
    if (IsKeyPressed(KEY_E)) { // Tecla 'E' para soltar a bomba
        if (game->player.bombCount > 0) {
            Bomb* availableBomb = NULL;
            // Encontra uma bomba inativa para usar
            for (int i = 0; i < MAX_BOMBS; i++) {
                if (!game->bombs[i].active) {
                    availableBomb = &game->bombs[i];
                    break;
                }
            }

            if (availableBomb != NULL) {
                InitBomb(availableBomb); // Re-inicializa a bomba para garantir um estado limpo
                availableBomb->position = game->player.position; // Posição atual do player
                availableBomb->active = true;
                game->player.bombCount--; // Decrementa o contador de bombas do player
                TraceLog(LOG_INFO, "GAME: Bomba solta em (%.2f, %.2f). Bombas restantes: %d",
                         availableBomb->position.x, availableBomb->position.y, game->player.bombCount);
            } else {
                TraceLog(LOG_WARNING, "GAME: Nenhuma bomba inativa disponível para soltar!");
            }
        } else {
            TraceLog(LOG_INFO, "GAME: Sem bombas para soltar! (Bombas: %d)", game->player.bombCount);
        }
    }

    // Lógica de spawn de inimigos
    game->enemySpawnTimer += dt;
    if (game->enemySpawnTimer >= game->enemySpawnInterval) {
        SpawnEnemy(game, GetScreenWidth(), GetScreenHeight());
        game->enemySpawnTimer = 0.0f; // Reseta o timer
    }

    // Atualiza inimigos
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) {
            UpdateEnemy(&game->enemies[i], dt, &game->player);

            // --- Colisão Inimigo vs. Inimigo ---
            for (int j = i + 1; j < MAX_ENEMIES; j++) {
                if (game->enemies[j].active) {
                    if (CheckCollisionRecs(game->enemies[i].hitbox, game->enemies[j].hitbox)) {
                        // Repulsão simples: Mova os inimigos para longe um do outro
                        Vector2 repulsionVec = Vector2Subtract(game->enemies[i].position, game->enemies[j].position);
                        float distance = Vector2Length(repulsionVec);

                        if (distance < 0.1f) { 
                            repulsionVec = (Vector2){ (float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100) };
                            distance = Vector2Length(repulsionVec);
                        }

                        if (distance > 0) {
                            repulsionVec = Vector2Normalize(repulsionVec);
                         
                            float overlap = (game->enemies[i].hitbox.width / 2 + game->enemies[j].hitbox.width / 2) - distance;
                            if (overlap > 0) {
                                game->enemies[i].position = Vector2Add(game->enemies[i].position, Vector2Scale(repulsionVec, overlap * 0.5f));
                                game->enemies[j].position = Vector2Subtract(game->enemies[j].position, Vector2Scale(repulsionVec, overlap * 0.5f));
                            }
                        }
                    }
                }
            }

            // --- Colisão Inimigo vs. Jogador ---
            if (CheckCollisionRecs(game->enemies[i].hitbox, game->player.hitbox)) {
                if (!game->player.isInvulnerable) {
                    TraceLog(LOG_INFO, "GAME: Colisão inimigo-player!");
                    int damage = 10; // Dano padrão
                    
                    PlayerTakeDamage(&game->player, damage);
                }
            }
        }
    }

    // Atualiza bombas e verifica dano
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (game->bombs[i].active) {
            UpdateBomb(&game->bombs[i], dt);

            // --- Lógica de Dano da Bomba aos Inimigos ---
            if (game->bombs[i].hasExploded && !game->bombs[i].damageApplied) {
                // Aplica o dano uma única vez quando a bomba explode
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (game->enemies[j].active) {
                        // Verifica colisão bomba com hitbox retangular inimigo
                        if (CheckCollisionCircleRec(game->bombs[i].position, game->bombs[i].damageRadius, game->enemies[j].hitbox)) {
                            EnemyTakeDamage(&game->enemies[j], game->bombs[i].damageAmount);
                        }
                    }
                }
                game->bombs[i].damageApplied = true; // Marca que o dano foi aplicado
                TraceLog(LOG_INFO, "GAME: Dano da bomba aplicado. Raio: %.2f", game->bombs[i].damageRadius);
            }
        }
    }
}

void DrawGame(const Game* game, const GameAssets* assets) {
    // Desenhar o mapa primeiro
    DrawMap(&game->map, assets);

    // Desenhar o player
    DrawPlayer(&game->player, assets);

    // Desenhar inimigos ativos
    for (int i = 0; i < MAX_ENEMIES; i++) {
        DrawEnemy(&game->enemies[i], assets);
    }

    // Desenhar bombas ativas
    for (int i = 0; i < MAX_BOMBS; i++) {
        DrawBomb(&game->bombs[i], assets);
    }

    // DEBUG: Mostrar vida do jogador e bombas
    DrawText(TextFormat("Health: %d", game->player.health), 10, 40, 20, LIME);
    DrawText(TextFormat("Bombs: %d / %d", game->player.bombCount, game->player.maxBombs), 10, 70, 20, ORANGE);
}

void UnloadGame(Game* game) {
    TraceLog(LOG_INFO, "GAME: Descarregando recursos do jogo (além dos assets globais).");
 
}

// --- Funções Auxiliares de Jogo ---

void SpawnEnemy(Game* game, int screenWidth, int screenHeight) {
    // Encontra um inimigo inativo no array
    Enemy* newEnemy = NULL;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game->enemies[i].active) {
            newEnemy = &game->enemies[i];
            break;
        }
    }

    if (newEnemy == NULL) {
        TraceLog(LOG_WARNING, "GAME: Não há inimigos inativos para spawnar!");
        return; // Não há inimigos disponíveis para spawnar
    }

    InitEnemy(newEnemy); 

    // Define uma posição inicial aleatória em um dos cantos da tela
    int corner = GetRandomValue(0, 3);
    float padding = 50.0f; // Margem para o inimigo não spawnar muito na borda

    switch (corner) {
        case 0: newEnemy->position = (Vector2){padding, padding}; break; // Top-Left
        case 1: newEnemy->position = (Vector2){(float)screenWidth - padding, padding}; break; // Top-Right
        case 2: newEnemy->position = (Vector2){padding, (float)screenHeight - padding}; break; // Bottom-Left
        case 3: newEnemy->position = (Vector2){(float)screenWidth - padding, (float)screenHeight - padding}; break; // Bottom-Right
    }

    newEnemy->active = true;
    TraceLog(LOG_INFO, "GAME: Inimigo spawnado na posição: (%.2f, %.2f)", newEnemy->position.x, newEnemy->position.y);
}

