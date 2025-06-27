#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "asset_manager.h" 
#include "player.h"       


typedef enum {
    ENEMY_STATE_RUN,
    ENEMY_STATE_DEATH
} EnemyAnimationState;

// Enum para a direção do olhar do inimigo (igual ao player para reuso de lógica)
typedef enum {
    ENEMY_DIR_DOWN = 0,
    ENEMY_DIR_LEFT = 1,
    ENEMY_DIR_RIGHT = 2,
    ENEMY_DIR_UP = 3
} EnemyDirection;


typedef struct {
    Vector2 position;
    Vector2 targetPosition; // Posição do alvo (geralmente o player)
    float speed;            // Velocidade de movimento do inimigo
    bool active;            // Se o inimigo está ativo no jogo

    EnemyAnimationState currentState;
    EnemyDirection currentDirection;

    int currentFrame;
    float frameTimer;
    float frameSpeed;

    Rectangle hitbox;        // Retângulo de colisão do inimigo

    // --- ATRIBUTOS PARA VIDA E INVULNERABILIDADE (para levar dano da bomba) ---
    int health;              // Vida do inimigo
    bool isInvulnerable;     // Se o inimigo está invulnerável após levar dano
    float invulnerableTimer; // Contador para a duração da invulnerabilidade
    float invulnerableDuration; // Duração total da invulnerabilidade

} Enemy;

// Declarações de funções do inimigo
void InitEnemy(Enemy* enemy);
void UpdateEnemy(Enemy* enemy, float dt, const Player* player);
void DrawEnemy(const Enemy* enemy, const GameAssets* assets);

// Funções auxiliares 
Rectangle GetEnemyHitbox(const Enemy* enemy);
void EnemyTakeDamage(Enemy* enemy, int damage); // Nova função para inimigo levar dano

#endif // ENEMY_H