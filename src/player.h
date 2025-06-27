#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "asset_manager.h" 

typedef enum {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_RUN,
    PLAYER_STATE_HIT_DEATH
} PlayerAnimationState;

// Enum para a direção do player
typedef enum {
    PLAYER_DIR_DOWN = 0,
    PLAYER_DIR_LEFT = 1,
    PLAYER_DIR_RIGHT = 2,
    PLAYER_DIR_UP = 3
} PlayerDirection;

typedef struct {
    Vector2 position;
    float speed; // Velocidade base do jogador
    PlayerAnimationState currentState;
    PlayerDirection currentDirection;

    int currentFrame;
    float frameTimer;
    float frameSpeed;

    // --- Atributos para colisão e vida ---
    int health;              // Vida do jogador
    bool isInvulnerable;     // Se o jogador está invulnerável após levar dano
    float invulnerableTimer; // Contador para a duração da invulnerabilidade
    float invulnerableDuration; // Duração total da invulnerabilidade
    Rectangle hitbox;        // Retângulo de colisão do jogador

    // --- Atributos para velocidade de escape ---
    float escapeSpeedBonus;  // Bônus de velocidade ao levar dano
    float escapeSpeedTimer;  // Contador para a duração do bônus de velocidade
    float escapeSpeedDuration; // Duração total do bônus de velocidade

    // --- ATRIBUTOS PARA BOMBAS ---
    int bombCount;              // Número de bombas que o player possui
    int maxBombs;               // Capacidade máxima de bombas
    float bombRechargeTimer;    // Timer para recarregar a próxima bomba
    float bombRechargeInterval; // Tempo para recarregar uma única bomba (e.g., 5.0s)

} Player;

// Declarações de funções do player
void InitPlayer(Player* player, Vector2 startPos);
void UpdatePlayer(Player* player, float dt);
void DrawPlayer(const Player* player, const GameAssets* assets);

// Funções auxiliares 
Rectangle GetPlayerHitbox(const Player* player);
void PlayerTakeDamage(Player* player, int damage);

#endif // PLAYER_H