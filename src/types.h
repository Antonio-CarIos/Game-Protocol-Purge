// src/types.h
#ifndef TYPES_H
#define TYPES_H

#include "raylib.h" // Apenas para tipos básicos como Vector2, Rectangle


// Definir o número máximo de inimigos e bombas
#define MAX_ENEMIES_CAPACITY 200
#define MAX_BOMBS 20 // Aumentado para suportar múltiplas bombas por clique
#define MAX_RANK_ENTRIES 10 // Limite de entradas no ranking
#define MAX_NAME_LENGTH 30 // Tamanho máximo para o nome do jogador


// --- ENUMS GLOBAIS ---
typedef enum {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_RUN,
    PLAYER_STATE_HIT_DEATH
} PlayerAnimationState;

typedef enum {
    PLAYER_DIR_DOWN = 0,
    PLAYER_DIR_RIGHT = 1,
    PLAYER_DIR_UP = 2,
    PLAYER_DIR_LEFT = 3
} PlayerDirection;

typedef enum {
    ENEMY_STATE_RUN,
    ENEMY_STATE_DEATH
} EnemyAnimationState;

typedef enum {
    ENEMY_DIR_DOWN = 0,
    ENEMY_DIR_LEFT = 1,
    ENEMY_DIR_RIGHT = 2,
    ENEMY_DIR_UP = 3
} EnemyDirection;

typedef enum {
    GAME_STATE_START_MENU,
    GAME_STATE_OPTIONS_MENU,
    GAME_STATE_NAME_INPUT,       // NOVO: Estado para entrada de nome
    GAME_STATE_LEVEL_UP_OPTIONS,
    GAME_STATE_PLAYING,
    GAME_STATE_DEATH,
    GAME_STATE_RANKING_VIEW      // NOVO: Estado para visualizar o ranking
} GameState;

typedef enum {
    MENU_OPTION_PLAY = 0,
    MENU_OPTION_RANK = 1,
    MENU_OPTION_EXIT = 2,
    BUFF_OPTION_1 = 0,
    BUFF_OPTION_2 = 1,
    BUFF_OPTION_3 = 2
} MenuOption;

typedef enum {
    BUFF_TYPE_SPEED = 0,
    BUFF_TYPE_HEALTH,
    BUFF_TYPE_DAMAGE,
    BUFF_TYPE_BOMB_CAP
} BuffType;


// --- STRUCTS GLOBAIS ---
// Definindo as structs COMPLETAS aqui
typedef struct {
    BuffType type;
    float value;
    const char* debugText;
} BuffEffect;

typedef struct Player {
    Vector2 position;
    float speed;
    PlayerAnimationState currentState;
    PlayerDirection currentDirection;
    int currentFrame;
    float frameTimer;
    float frameSpeed;
    int health;
    int maxHealth;
    bool isInvulnerable;
    float invulnerableTimer;
    float invulnerableDuration;
    Rectangle hitbox;
    float escapeSpeedBonus;
    float escapeSpeedTimer;
    float escapeSpeedDuration;
    int bombCount;
    int maxBombs;
    float bombRechargeTimer;
    float bombRechargeInterval;
    int bombsPerClick;
    int currentXP;
    float baseDamageMultiplier;
    int bombBaseDamage;
    char name[MAX_NAME_LENGTH + 1]; // NOVO: Nome do jogador
} Player;

typedef struct Enemy {
    Vector2 position;
    Vector2 targetPosition;
    float speed;
    bool active;
    EnemyAnimationState currentState;
    EnemyDirection currentDirection;
    int currentFrame;
    float frameTimer;
    float frameSpeed;
    Rectangle hitbox;
    int health;
    bool isInvulnerable;
    float invulnerableTimer;
    float invulnerableDuration;
    int xpValue;
} Enemy;

typedef struct Bomb {
    Vector2 position;
    bool active;
    float activationTimer;
    float activationDuration;
    int currentFrame;
    float frameTimer;
    float frameSpeed;
    int frameCount;
    bool hasExploded;
    float damageRadius;
    int damageAmount;
    bool damageApplied;
    Rectangle explosionSourceRecs[13];
} Bomb;

typedef struct GameMap {
    // Adicione quaisquer membros que GameMap precisa aqui
} GameMap;

// NOVO: Struct para uma entrada no ranking
typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    int level;
    float survivalTime;
} RankEntry;

// A struct Game agora armazenará PONTEIROS para as outras structs
typedef struct {
    Player* player;
    Enemy* enemies[MAX_ENEMIES_CAPACITY];
    Bomb* bombs[MAX_BOMBS];
    GameMap* map;

    float enemySpawnTimer;
    float enemySpawnInterval;
    int activeMaxEnemies;
    int currentRound;
    int xpRequiredForNextRound;

    GameState currentGameState;
    float gameTimer;

    int startScreenCurrentFrame;
    float startScreenFrameTimer;
    float startScreenFrameSpeed;
    int startScreenFrameCount;
    Texture2D startScreenTexture;

    MenuOption currentMenuOption;
    float menuInputDelay;
    float menuInputTimer;

    int currentBuffScreen;
    MenuOption currentBuffOption;
    BuffEffect selectedBuffs[3];

    // NOVO: Membros para a entrada de nome
    char inputNameBuffer[MAX_NAME_LENGTH + 1];
    int inputNameLength;
    float cursorBlinkTimer;
    bool showCursor;

    // NOVO: Membros para o ranking
    RankEntry ranking[MAX_RANK_ENTRIES];
    int currentRankEntries;

} Game;

#endif // TYPES_H