#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "asset_manager.h"
#include "player.h"
#include "enemy.h"
#include "bomb.h"
#include "map.h"

// Definir o número máximo de inimigos e bombas
#define MAX_ENEMIES_CAPACITY 200
#define MAX_BOMBS 2

// Enum para gerenciar os estados do jogo (telas)
typedef enum {
    GAME_STATE_START_MENU, // Tela de menu inicial
    GAME_STATE_OPTIONS_MENU, // Tela de menu de opções (Play, Rank, Exit)
    GAME_STATE_LEVEL_UP_OPTIONS, // NOVO: Tela de opções de buff após subir de round
    GAME_STATE_PLAYING,    // Jogo rolando normalmente
    GAME_STATE_DEATH       // Tela de morte (Game Over)
} GameState;

// Enum para as opções do menu (reaproveitado para buffs)
typedef enum {
    MENU_OPTION_PLAY = 0,
    MENU_OPTION_RANK = 1,
    MENU_OPTION_EXIT = 2,
    // NOVO: Mapeamento para as opções de buff (opção 0, 1, 2 dentro de cada tela de buff)
    BUFF_OPTION_1 = 0,
    BUFF_OPTION_2 = 1,
    BUFF_OPTION_3 = 2
} MenuOption; // Renomeei para algo mais genérico ou criar um novo enum BuffOption

// Estrutura para definir um buff específico
typedef struct {
    int type;  // 0=speed, 1=health, 2=damage, 3=bombCapacity
    float value; // Valor do buff
    // Adicionar um identificador ou texto para debug, se necessário
    char debugText[32]; // Ex: "+5% SPD", "+1 HP", "+1 BOMB", "+5% DMG"
} BuffEffect;

typedef struct {
    Player player;
    Enemy enemies[MAX_ENEMIES_CAPACITY];
    Bomb bombs[MAX_BOMBS];
    GameMap map;

    float enemySpawnTimer;
    float enemySpawnInterval;
    int activeMaxEnemies;

    int currentRound;
    int xpRequiredForNextRound;

    // Atributos do jogo para controle de estado e tempo
    GameState currentGameState;
    float gameTimer;

    // Atributos para a animação da tela de início
    int startScreenCurrentFrame;
    float startScreenFrameTimer;
    float startScreenFrameSpeed;
    int startScreenFrameCount;
    Texture2D startScreenTexture; // Referencia g_gameAssets.startScreenAnimationSheet

    // Atributos para o menu de opções (PLAY, RANK, EXIT)
    MenuOption currentMenuOption;
    float menuInputDelay;
    float menuInputTimer;

    // NOVO: Atributos para o menu de buffs (LEVEL_UP_OPTIONS)
    int currentBuffScreen;     // Qual das 3 telas de buff aleatórias está ativa (0, 1 ou 2)
    MenuOption currentBuffOption; // Qual opção está selecionada na tela de buff (0, 1 ou 2)
    BuffEffect selectedBuffs[3]; // As 3 opções de buff disponíveis na tela atual

} Game;

// Declarações de funções do jogo
void InitGame(Game* game, Vector2 playerStartPos);
void UpdateGame(Game* game, float dt);
void DrawGame(const Game* game, const GameAssets* assets);
void UnloadGame(Game* game);

// Funções auxiliares para o jogo
void SpawnEnemy(Game* game);
void LevelUpPlayer(Game* game);
void GenerateBuffOptions(Game* game); // NOVO: Gera as opções de buff aleatórias

#endif // GAME_H