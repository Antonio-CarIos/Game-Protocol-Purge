// src/game.c
#include "game.h"       // Inclui game.h, que por sua vez inclui types.h e asset_manager.h
#include "player.h"     // Para InitPlayer, UpdatePlayer, DrawPlayer, PlayerApplyBuff, PlayerTakeDamage
#include "enemy.h"      // Para InitEnemy, UpdateEnemy, DrawEnemy, EnemyTakeDamage
#include "bomb.h"       // Para InitBomb, UpdateBomb, DrawBomb
#include "map.h"        // Para InitMap, DrawMap

#include "raylib.h"
#include "raymath.h"
#include <stdlib.h> // Para malloc, free, srand, qsort
#include <time.h>   // Para time
#include <math.h>   // Para ceil(), fabs()
#include <stdio.h>  // Para snprintf, TraceLog, FILE I/O
#include <string.h> // Para strcmp, strcpy, strlen


// --- Constantes e Funções Auxiliares (Estáticas) ---
#define RANKING_FILENAME "ranking.txt" // Nome do arquivo de ranking na raiz do projeto

static BuffEffect buffScreens[3][3] = {
    { {BUFF_TYPE_DAMAGE, 5.0f, "+5% Dmg"}, {BUFF_TYPE_HEALTH, 20.0f, "+1 Health"}, {BUFF_TYPE_SPEED, 5.0f, "+5% Speed"} },
    { {BUFF_TYPE_DAMAGE, 5.0f, "+5% Dmg"}, {BUFF_TYPE_HEALTH, 20.0f, "+1 Health"}, {BUFF_TYPE_BOMB_CAP, 1.0f, "+1 Bomb"} },
    { {BUFF_TYPE_DAMAGE, 5.0f, "+5% Dmg"}, {BUFF_TYPE_HEALTH, 40.0f, "+2 Health"}, {BUFF_TYPE_SPEED, 7.0f, "+7% Speed"} }
};

static Rectangle buffSourceRecs[3][3] = {
    {   {6.0f, 2.0f, 113.0f, 83.0f},   // Opção 0
        {121.0f, 2.0f, 113.0f, 83.0f}, // Opção 1
        {237.0f, 2.0f, 113.0f, 83.0f}  // Opção 2
    },
    {   {6.0f, 88.0f, 113.0f, 83.0f},   // Opção 0
        {120.0f, 88.0f, 113.0f, 83.0f}, // Opção 1
        {236.0f, 88.0f, 113.0f, 83.0f}  // Opção 2
    },
    {   {7.0f, 173.0f, 113.0f, 83.0f},   // Opção 0
        {120.0f, 173.0f, 113.0f, 83.0f}, // Opção 1
        {237.0f, 173.0f, 113.0f, 83.0f}  // Opção 2
    }
};

// Função de comparação para qsort (maior nível primeiro, depois maior tempo)
static int CompareRankEntries(const void* a, const void* b) {
    RankEntry* entryA = (RankEntry*)a;
    RankEntry* entryB = (RankEntry*)b;

    if (entryA->level != entryB->level) {
        return entryB->level - entryA->level; // Nível maior vem primeiro
    }
    // Se os níveis são iguais, compara pelo tempo de sobrevivência
    if (entryA->survivalTime < entryB->survivalTime) {
        return 1; // entryA vem depois de entryB
    }
    if (entryA->survivalTime > entryB->survivalTime) {
        return -1; // entryA vem antes de entryB
    }
    return 0; // São iguais
}


void InitGame(Game* game, Vector2 playerStartPos) {
    TraceLog(LOG_INFO, "GAME: Inicializando jogo...");

    // Alocar memória para as structs
    game->player = (Player*)malloc(sizeof(Player));
    if (game->player == NULL) { TraceLog(LOG_FATAL, "GAME: Falha ao alocar Player!"); exit(1); }

    game->map = (GameMap*)malloc(sizeof(GameMap));
    if (game->map == NULL) { TraceLog(LOG_FATAL, "GAME: Falha ao alocar GameMap!"); exit(1); }

    for (int i = 0; i < MAX_ENEMIES_CAPACITY; i++) {
        game->enemies[i] = (Enemy*)malloc(sizeof(Enemy));
        if (game->enemies[i] == NULL) { TraceLog(LOG_FATAL, "GAME: Falha ao alocar Enemy %d!", i); exit(1); }
    }

    for (int i = 0; i < MAX_BOMBS; i++) {
        game->bombs[i] = (Bomb*)malloc(sizeof(Bomb));
        if (game->bombs[i] == NULL) { TraceLog(LOG_FATAL, "GAME: Falha ao alocar Bomb %d!", i); exit(1); }
    }

    // Inicializar os conteúdos das structs alocadas
    InitPlayer(game->player, playerStartPos);
    InitMap(game->map);

    for (int i = 0; i < MAX_ENEMIES_CAPACITY; i++) {
        InitEnemy(game->enemies[i]);
    }
    game->enemySpawnTimer = 0.0f;
    game->enemySpawnInterval = 3.0f;
    game->activeMaxEnemies = 5;

    for (int i = 0; i < MAX_BOMBS; i++) {
        InitBomb(game->bombs[i]);
    }

    game->currentRound = 1;
    game->xpRequiredForNextRound = 100;

    game->currentGameState = GAME_STATE_START_MENU;
    game->gameTimer = 0.0f;

    game->startScreenCurrentFrame = 0;
    game->startScreenFrameTimer = 0.0f;
    game->startScreenFrameSpeed = 0.1f;
    game->startScreenFrameCount = 16;

    game->currentMenuOption = MENU_OPTION_PLAY;
    game->menuInputDelay = 0.2f;
    game->menuInputTimer = 0.0f;

    game->currentBuffScreen = 0;
    game->currentBuffOption = BUFF_OPTION_1;

    // NOVO: Inicialização para entrada de nome
    memset(game->inputNameBuffer, 0, sizeof(game->inputNameBuffer));
    game->inputNameLength = 0;
    game->cursorBlinkTimer = 0.0f;
    game->showCursor = true;

    // NOVO: Inicialização do ranking
    game->currentRankEntries = 0;
    LoadRanking(game, RANKING_FILENAME); // Carrega o ranking ao iniciar o jogo

    srand((unsigned int)time(NULL));
    TraceLog(LOG_INFO, "GAME: Jogo inicializado.");
}

void UpdateGame(Game* game, float dt) {
    game->menuInputTimer += dt;

    switch (game->currentGameState) {
        case GAME_STATE_START_MENU:
            game->startScreenFrameTimer += dt;
            if (game->startScreenFrameTimer >= game->startScreenFrameSpeed) {
                game->startScreenFrameTimer = 0.0f;
                game->startScreenCurrentFrame++;
                if (game->startScreenCurrentFrame >= game->startScreenFrameCount) {
                    game->startScreenCurrentFrame = 0;
                }
            }
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                game->currentGameState = GAME_STATE_OPTIONS_MENU;
                TraceLog(LOG_INFO, "MAIN: Transicionando para GAME_STATE_OPTIONS_MENU ao pressionar SPACE ou ENTER.");
            }
            break;

        case GAME_STATE_OPTIONS_MENU:
            if (game->menuInputTimer >= game->menuInputDelay) {
                if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
                    game->currentMenuOption++;
                    if (game->currentMenuOption > MENU_OPTION_EXIT) {
                        game->currentMenuOption = MENU_OPTION_PLAY;
                    }
                    game->menuInputTimer = 0.0f;
                    TraceLog(LOG_INFO, "MENU: Opcao selecionada: %d", game->currentMenuOption);
                } else if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
                    game->currentMenuOption--;
                    if (game->currentMenuOption < MENU_OPTION_PLAY) {
                        game->currentMenuOption = MENU_OPTION_EXIT;
                    }
                    game->menuInputTimer = 0.0f;
                    TraceLog(LOG_INFO, "MENU: Opcao selecionada: %d", game->currentMenuOption);
                } else if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    switch (game->currentMenuOption) {
                        case MENU_OPTION_PLAY:
                            // Redefinir o player para um novo jogo
                            InitPlayer(game->player, (Vector2){ GetScreenWidth() / 2, GetScreenHeight() / 2 });
                            game->currentRound = 1;
                            game->xpRequiredForNextRound = 100;
                            game->gameTimer = 0.0f; // Reiniciar o timer
                            for (int i = 0; i < MAX_ENEMIES_CAPACITY; i++) {
                                InitEnemy(game->enemies[i]); // Reinicializa todos os inimigos
                            }
                            game->enemySpawnTimer = 0.0f;
                            game->activeMaxEnemies = 5;
                            for (int i = 0; i < MAX_BOMBS; i++) {
                                InitBomb(game->bombs[i]); // Reinicializa todas as bombas
                            }

                            game->currentGameState = GAME_STATE_NAME_INPUT; // NOVO: Vai para a tela de nome
                            TraceLog(LOG_INFO, "MENU: Selecionado PLAY. Transicionando para GAME_STATE_NAME_INPUT.");
                            break;
                        case MENU_OPTION_RANK:
                            game->currentGameState = GAME_STATE_RANKING_VIEW; // NOVO: Vai para a tela de ranking
                            TraceLog(LOG_INFO, "MENU: Selecionado RANK. Transicionando para GAME_STATE_RANKING_VIEW.");
                            break;
                        case MENU_OPTION_EXIT:
                            TraceLog(LOG_INFO, "MENU: Selecionado EXIT. Fechando o jogo.");
                            CloseWindow();
                            break;
                    }
                }
            }
            break;

        // NOVO: Lógica para entrada de nome do jogador
        case GAME_STATE_NAME_INPUT: {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && (game->inputNameLength < MAX_NAME_LENGTH)) {
                    game->inputNameBuffer[game->inputNameLength] = (char)key;
                    game->inputNameLength++;
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (game->inputNameLength > 0) {
                    game->inputNameLength--;
                    game->inputNameBuffer[game->inputNameLength] = '\0';
                }
            }

            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                if (game->inputNameLength > 0) {
                    strcpy(game->player->name, game->inputNameBuffer);
                    game->currentGameState = GAME_STATE_PLAYING;
                    TraceLog(LOG_INFO, "NAME_INPUT: Nome do jogador definido para: %s", game->player->name);
                } else {
                    strcpy(game->player->name, "Player"); // Nome padrão se nada for digitado
                    game->currentGameState = GAME_STATE_PLAYING;
                    TraceLog(LOG_INFO, "NAME_INPUT: Nenhum nome digitado. Usando nome padrao: Player");
                }
            }

            // Piscar do cursor
            game->cursorBlinkTimer += dt;
            if (game->cursorBlinkTimer >= 0.5f) {
                game->showCursor = !game->showCursor;
                game->cursorBlinkTimer = 0.0f;
            }
            break;
        }

        case GAME_STATE_LEVEL_UP_OPTIONS:
            if (game->menuInputTimer >= game->menuInputDelay) {
                if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
                    game->currentBuffOption++;
                    if (game->currentBuffOption > BUFF_OPTION_3) {
                        game->currentBuffOption = BUFF_OPTION_1;
                    }
                    game->menuInputTimer = 0.0f;
                    TraceLog(LOG_INFO, "LEVEL_UP_MENU: Opcao selecionada: %d", game->currentBuffOption);
                } else if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
                    game->currentBuffOption--;
                    if (game->currentBuffOption < BUFF_OPTION_1) {
                        game->currentBuffOption = BUFF_OPTION_3;
                    }
                    game->menuInputTimer = 0.0f;
                    TraceLog(LOG_INFO, "LEVEL_UP_MENU: Opcao selecionada: %d", game->currentBuffOption);
                } else if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    BuffEffect chosenBuff = game->selectedBuffs[game->currentBuffOption];
                    PlayerApplyBuff(game->player, chosenBuff.type, chosenBuff.value);
                    TraceLog(LOG_INFO, "LEVEL_UP_MENU: Buff aplicado: %s", chosenBuff.debugText);
                    game->currentGameState = GAME_STATE_PLAYING;
                    TraceLog(LOG_INFO, "LEVEL_UP_MENU: Buff selecionado. Retornando ao jogo.");
                }
            }
            break;

        case GAME_STATE_PLAYING:
            game->gameTimer += dt;
            UpdatePlayer(game->player, dt);

            if (IsKeyPressed(KEY_E)) {
                if (game->player->bombCount > 0) {
                    for (int k = 0; k < game->player->bombsPerClick; k++) {
                        Bomb* availableBomb = NULL;
                        for (int i = 0; i < MAX_BOMBS; i++) {
                            if (!game->bombs[i]->active) {
                                availableBomb = game->bombs[i];
                                break;
                            }
                        }

                        if (availableBomb != NULL) {
                            InitBomb(availableBomb);
                            Vector2 spawnOffset = {0, 0};
                            if (game->player->bombsPerClick > 1) {
                                float horizontalOffset = 30.0f;
                                if (k == 0) {
                                    spawnOffset.x = -horizontalOffset;
                                } else if (k == 1) {
                                    spawnOffset.x = horizontalOffset;
                                }
                            }
                            availableBomb->position = Vector2Add(game->player->position, spawnOffset);
                            availableBomb->active = true;
                            availableBomb->damageAmount = (int)(game->player->bombBaseDamage * game->player->baseDamageMultiplier);
                            TraceLog(LOG_INFO, "GAME: Bomba #%d solta em (%.2f, %.2f). (Dano: %d)",
                                     k + 1, availableBomb->position.x, availableBomb->position.y, availableBomb->damageAmount);
                        } else {
                            TraceLog(LOG_WARNING, "GAME: Nenhuma bomba inativa disponível para soltar!");
                            break;
                        }
                    }
                    game->player->bombCount--;
                } else {
                    TraceLog(LOG_INFO, "GAME: Sem bombas para soltar! (Bombas: %d)", game->player->bombCount);
                }
            }

            game->enemySpawnTimer += dt;
            if (game->enemySpawnTimer >= game->enemySpawnInterval) {
                int currentActiveEnemies = 0;
                for (int i = 0; i < MAX_ENEMIES_CAPACITY; i++) {
                    if (game->enemies[i]->active) {
                        currentActiveEnemies++;
                    }
                }

                if (currentActiveEnemies < game->activeMaxEnemies) {
                    SpawnEnemy(game);
                }
                game->enemySpawnTimer = 0.0f;
            }

            for (int i = 0; i < MAX_ENEMIES_CAPACITY; i++) {
                if (game->enemies[i]->active) {
                    UpdateEnemy(game->enemies[i], dt, game->player);

                    for (int j = i + 1; j < MAX_ENEMIES_CAPACITY; j++) {
                        if (game->enemies[j]->active) {
                            if (CheckCollisionRecs(game->enemies[i]->hitbox, game->enemies[j]->hitbox)) {
                                Vector2 repulsionVec = Vector2Subtract(game->enemies[i]->position, game->enemies[j]->position);
                                float distance = Vector2Length(repulsionVec);
                                if (distance < 0.1f) {
                                    repulsionVec = (Vector2){ (float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100) };
                                    distance = Vector2Length(repulsionVec);
                                }
                                if (distance > 0) {
                                    repulsionVec = Vector2Normalize(repulsionVec);
                                    float overlap = (game->enemies[i]->hitbox.width / 2 + game->enemies[j]->hitbox.width / 2) - distance;
                                    if (overlap > 0) {
                                        game->enemies[i]->position = Vector2Add(game->enemies[i]->position, Vector2Scale(repulsionVec, overlap * 0.5f));
                                        game->enemies[j]->position = Vector2Subtract(game->enemies[j]->position, Vector2Scale(repulsionVec, overlap * 0.5f));
                                    }
                                }
                            }
                        }
                    }

                    if (CheckCollisionRecs(game->enemies[i]->hitbox, game->player->hitbox)) {
                        if (!game->player->isInvulnerable) {
                            TraceLog(LOG_INFO, "GAME: Colisão inimigo-player!");
                            int damage = 10;
                            PlayerTakeDamage(game->player, damage);
                        }
                    }
                }
            }

            for (int i = 0; i < MAX_BOMBS; i++) {
                if (game->bombs[i]->active) {
                    UpdateBomb(game->bombs[i], dt);

                    if (game->bombs[i]->hasExploded && game->bombs[i]->damageApplied) {
                        for (int j = 0; j < MAX_ENEMIES_CAPACITY; j++) {
                            if (game->enemies[j]->active) {
                                if (CheckCollisionCircleRec(game->bombs[i]->position, game->bombs[i]->damageRadius, game->enemies[j]->hitbox)) {
                                    int xpGained = EnemyTakeDamage(game->enemies[j], game->bombs[i]->damageAmount);
                                    if (xpGained > 0) {
                                        game->player->currentXP += xpGained;
                                        TraceLog(LOG_INFO, "GAME: Player ganhou %d XP. Total: %d", xpGained, game->player->currentXP);
                                    }
                                }
                            }
                        }
                        game->bombs[i]->damageApplied = false;
                        TraceLog(LOG_INFO, "GAME: Dano da bomba aplicado. Raio: %.2f", game->bombs[i]->damageRadius);
                    }
                }
            }

            if (game->player->currentXP >= game->xpRequiredForNextRound) {
                LevelUpPlayer(game);
            }

            if (game->player->health <= 0) {
                game->currentGameState = GAME_STATE_DEATH;
                // NOVO: Salvar a pontuação ao morrer
                AddScoreToRanking(game, game->player->name, game->currentRound, game->gameTimer);
                SaveRanking(game, RANKING_FILENAME);
                TraceLog(LOG_INFO, "GAME: Game Over! Player morreu!");
            }
            break;

        case GAME_STATE_DEATH:
            // Apenas aguarda input para reiniciar, o ranking já foi salvo.
            break;

        // NOVO: Lógica para visualização do ranking
        case GAME_STATE_RANKING_VIEW:
            if (IsKeyPressed(KEY_ESCAPE)) {
                game->currentGameState = GAME_STATE_OPTIONS_MENU; // Volta para o menu de opções
                game->menuInputTimer = 0.0f; // Reseta o timer para evitar seleção acidental
                TraceLog(LOG_INFO, "RANKING_VIEW: Pressionou ESC. Voltando para OPTIONS_MENU.");
            }
            break;
    }
}

void DrawGame(const Game* game, const GameAssets* assets) {
    switch (game->currentGameState) {
        case GAME_STATE_START_MENU:
            if (game->startScreenTexture.id != 0) {
                Rectangle sourceRec = {
                    (float)game->startScreenCurrentFrame * GetScreenWidth(),
                    0.0f,
                    (float)GetScreenWidth(),
                    (float)GetScreenHeight()
                };
                DrawTexturePro(game->startScreenTexture, sourceRec,
                               (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
                               (Vector2){0, 0}, 0.0f, WHITE);
            } else {
                DrawText("START MENU (Pressione ESPAÇO ou ENTER para iniciar)", GetScreenWidth() / 2 - MeasureText("START MENU (Pressione ESPAÇO ou ENTER para iniciar)", 30) / 2, GetScreenHeight() / 2 - 15, 30, RAYWHITE);
            }
            break;

        case GAME_STATE_OPTIONS_MENU: {
            Rectangle menuSourceRecs[] = {
                {380.0f, 2.0f, 115.0f, 82.0f},
                {379.0f, 89.0f, 115.0f, 82.0f},
                {380.0f, 175.0f, 115.0f, 82.0f}
            };
            int frameIndex = (int)game->currentMenuOption;
            if (frameIndex < 0 || frameIndex >= sizeof(menuSourceRecs) / sizeof(menuSourceRecs[0])) {
                frameIndex = 0;
            }
            Rectangle currentSourceRec = menuSourceRecs[frameIndex];
            float menuScale = 3.0f;
            float menuWidth = currentSourceRec.width * menuScale;
            float menuHeight = currentSourceRec.height * menuScale;
            float posX = GetScreenWidth() / 2 - menuWidth / 2;
            float posY = GetScreenHeight() / 2 - menuHeight / 2;
            Rectangle destRec = { posX, posY, menuWidth, menuHeight };
            DrawTexturePro(assets->optionsMenuSheet, currentSourceRec, destRec, (Vector2){0,0}, 0.0f, WHITE);
            break;
        }

        // NOVO: Desenhar a tela de entrada de nome
        case GAME_STATE_NAME_INPUT: {
            ClearBackground(BLACK);
            int screenWidth = GetScreenWidth();
            int screenHeight = GetScreenHeight();

            const char* message = "Digite seu nome:";
            float messageWidth = MeasureText(message, 40);
            DrawText(message, screenWidth / 2 - messageWidth / 2, screenHeight / 2 - 80, 40, RAYWHITE);

            char displayBuffer[MAX_NAME_LENGTH + 2]; // +1 para o cursor, +1 para null terminator
            strcpy(displayBuffer, game->inputNameBuffer);
            if (game->showCursor) {
                displayBuffer[game->inputNameLength] = '_';
                displayBuffer[game->inputNameLength + 1] = '\0';
            } else {
                displayBuffer[game->inputNameLength] = '\0';
            }

            float inputWidth = MeasureText(displayBuffer, 30);
            DrawText(displayBuffer, screenWidth / 2 - inputWidth / 2, screenHeight / 2, 30, LIME);

            DrawText("Pressione ENTER ou ESPAÇO para continuar", screenWidth / 2 - MeasureText("Pressione ENTER ou ESPAÇO para continuar", 20) / 2, screenHeight / 2 + 80, 20, GRAY);
            break;
        }

        case GAME_STATE_LEVEL_UP_OPTIONS: {
            DrawMap(game->map, assets);
            DrawPlayer(game->player, assets);
            for (int i = 0; i < MAX_ENEMIES_CAPACITY; i++) {
                DrawEnemy(game->enemies[i], assets);
            }
            for (int i = 0; i < MAX_BOMBS; i++) {
                DrawBomb(game->bombs[i], assets);
            }

            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

            int screenIndex = game->currentBuffScreen;
            int optionIndex = (int)game->currentBuffOption;
            if (screenIndex < 0 || screenIndex >= 3) screenIndex = 0;
            if (optionIndex < 0 || optionIndex >= 3) optionIndex = 0;

            Rectangle currentSourceRec = buffSourceRecs[screenIndex][optionIndex];
            float buffMenuScale = 3.0f;
            float buffMenuWidth = currentSourceRec.width * buffMenuScale;
            float buffMenuHeight = currentSourceRec.height * buffMenuScale;
            float posX = GetScreenWidth() / 2 - buffMenuWidth / 2;
            float posY = GetScreenHeight() / 2 - buffMenuHeight / 2;
            Rectangle destRec = { posX, posY, buffMenuWidth, buffMenuHeight };
            DrawTexturePro(assets->optionsMenuSheet, currentSourceRec, destRec, (Vector2){0,0}, 0.0f, WHITE);

            float debugTextY = posY + buffMenuHeight + 20;
            for (int i = 0; i < 3; ++i) {
                char text[128];
                BuffEffect buff = game->selectedBuffs[i];
                snprintf(text, sizeof(text), "Opcao %d: %s", i + 1, buff.debugText);
                DrawText(text, posX, debugTextY + (i * 25), 20, LIGHTGRAY);
            }
            DrawText(TextFormat("DEBUG - Health: %d / %d", game->player->health, game->player->maxHealth), posX, debugTextY + (3 * 25) + 20, 20, YELLOW);
            DrawText(TextFormat("DEBUG - Max Bombs: %d", game->player->maxBombs), posX, debugTextY + (4 * 25) + 20, 20, YELLOW);
            DrawText(TextFormat("DEBUG - Bombs per Click: %d", game->player->bombsPerClick), posX, debugTextY + (5 * 25) + 20, 20, YELLOW);
            break;
        }

        case GAME_STATE_PLAYING:
            DrawMap(game->map, assets);
            DrawPlayer(game->player, assets);
            for (int i = 0; i < MAX_ENEMIES_CAPACITY; i++) {
                DrawEnemy(game->enemies[i], assets);
            }
            for (int i = 0; i < MAX_BOMBS; i++) {
                DrawBomb(game->bombs[i], assets);
            }

            float hudScale = 3.0f;
            if (assets->hudsSheet.id != 0) {
                Rectangle healthSourceRec = { 0 };
                Vector2 healthDrawPos = { 150, 40 };

                int healthSegments = 5;
                float healthPerSegment = (float)game->player->maxHealth / healthSegments;
                int currentVisibleSegments = (int)ceil((float)game->player->health / healthPerSegment);
                if (currentVisibleSegments < 0) currentVisibleSegments = 0;
                if (currentVisibleSegments > healthSegments) currentVisibleSegments = healthSegments;
                int healthFrameIndex = healthSegments - currentVisibleSegments;
                if (healthFrameIndex < 0) healthFrameIndex = 0;
                if (healthFrameIndex > healthSegments) healthFrameIndex = healthSegments;

                healthSourceRec = (Rectangle){ 9.0f, 74.0f + (healthFrameIndex * 12.0f), 41.0f, 12.0f };
                Rectangle healthDestRec = { healthDrawPos.x, healthDrawPos.y, healthSourceRec.width * hudScale, healthSourceRec.height * hudScale };
                DrawTexturePro(assets->hudsSheet, healthSourceRec, healthDestRec, (Vector2){0,0}, 0.0f, WHITE);

                Rectangle bombSourceRec = { 0 };
                Vector2 bombDrawPos = { healthDrawPos.x + (healthSourceRec.width * hudScale) + 20, healthDrawPos.y };
                int bombFrameIndex = game->player->bombCount;
                if (bombFrameIndex < 0) bombFrameIndex = 0;
                if (bombFrameIndex > 5) bombFrameIndex = 5;
                bombSourceRec = (Rectangle){ 114.0f, 87.0f + (5 - bombFrameIndex) * 11.0f, 42.0f, 10.0f };
                Rectangle bombDestRec = { bombDrawPos.x, bombDrawPos.y, bombSourceRec.width * hudScale, bombSourceRec.height * hudScale / 1.0f };
                DrawTexturePro(assets->hudsSheet, bombSourceRec, bombDestRec, (Vector2){0,0}, 0.0f, WHITE);

                Rectangle xpSourceRec = { 0 };
                Vector2 xpDrawPos = { GetScreenWidth() / 2 - (135 * hudScale / 2), 120 };
                int xpFrameIndex = 0;
                if (game->xpRequiredForNextRound > 0) {
                    float xpPercentage = (float)game->player->currentXP / game->xpRequiredForNextRound;
                    int filledSegments = (int)(xpPercentage * 22.0f);
                    if (filledSegments < 0) filledSegments = 0;
                    if (filledSegments > 22) filledSegments = 22;
                    xpFrameIndex = filledSegments;
                }
                xpSourceRec = (Rectangle){ 192.0f, 122.0f + (xpFrameIndex * 8.0f), 135.0f, 6.0f };
                Rectangle xpDestRec = { xpDrawPos.x, xpDrawPos.y, xpSourceRec.width * hudScale, xpSourceRec.height * hudScale };
                DrawTexturePro(assets->hudsSheet, xpSourceRec, xpDestRec, (Vector2){0,0}, 0.0f, WHITE);
            } else {
                DrawText(TextFormat("Health: %d / %d", game->player->health, game->player->maxHealth), 10, 40, 20, LIME);
                DrawText(TextFormat("Bombs: %d / %d (x%d per click)", game->player->bombCount, game->player->maxBombs, game->player->bombsPerClick), 10, 70, 20, ORANGE);
                DrawText(TextFormat("XP: %d / %d", game->player->currentXP, game->xpRequiredForNextRound), 10, 130, 20, GOLD);
            }

            // Draw player name during game
            char nameDisplay[MAX_NAME_LENGTH + 20];
            snprintf(nameDisplay, sizeof(nameDisplay), "Player: %s", game->player->name);
            DrawText(nameDisplay, 10, 10, 20, RAYWHITE);


            DrawText(TextFormat("Round: %d", game->currentRound), GetScreenWidth() / 2 - MeasureText(TextFormat("Round: %d", game->currentRound), 20) / 2, 90, 20, RAYWHITE);
            DrawText(TextFormat("TIME: %.2f", game->gameTimer), 10, 160, 20, YELLOW);

            DrawText(TextFormat("DEBUG - Health: %d / %d", game->player->health, game->player->maxHealth), 10, GetScreenHeight() - 100, 20, YELLOW);
            DrawText(TextFormat("DEBUG - Speed: %.2f", game->player->speed), 10, GetScreenHeight() - 70, 20, PURPLE);
            DrawText(TextFormat("DEBUG - Base Damage Multiplier: %.2f", game->player->baseDamageMultiplier), 10, GetScreenHeight() - 50, 20, PURPLE);
            DrawText(TextFormat("DEBUG - Bomb Base Damage: %d", game->player->bombBaseDamage), 10, GetScreenHeight() - 30, 20, PURPLE);
            DrawText(TextFormat("DEBUG - Bombs per click: %d", game->player->bombsPerClick), 10, GetScreenHeight() - 10, 20, PURPLE);
            break;

        case GAME_STATE_DEATH:
            DrawText("VOCÊ MORREU!", GetScreenWidth() / 2 - MeasureText("VOCÊ MORREU!", 40) / 2, GetScreenHeight() / 2 - 40, 40, RED);
            DrawText(TextFormat("Tempo de Sobrevivência: %.2f segundos", game->gameTimer),
                             GetScreenWidth() / 2 - MeasureText(TextFormat("Tempo de Sobrevivência: %.2f segundos", game->gameTimer), 20) / 2,
                             GetScreenHeight() / 2 + 20, 20, RAYWHITE);
            DrawText("Pressione R para Reiniciar", GetScreenWidth() / 2 - MeasureText("Pressione R para Reiniciar", 20) / 2, GetScreenHeight() - 50, 20, GRAY);
            break;

        // NOVO: Desenhar a tela de ranking
        case GAME_STATE_RANKING_VIEW: {
            ClearBackground(BLACK);
            int screenWidth = GetScreenWidth();
            int screenHeight = GetScreenHeight();

            DrawText("RANKING", screenWidth / 2 - MeasureText("RANKING", 50) / 2, 50, 50, GOLD);

            int startY = 150;
            int lineHeight = 30;

            if (game->currentRankEntries == 0) {
                DrawText("Nenhum ranking salvo ainda.", screenWidth / 2 - MeasureText("Nenhum ranking salvo ainda.", 30) / 2, startY, 30, GRAY);
            } else {
                for (int i = 0; i < game->currentRankEntries; i++) {
                    char rankText[256];
                    snprintf(rankText, sizeof(rankText), "%2d. %-20s Nivel: %-5d Tempo: %.2fs",
                             i + 1, game->ranking[i].name, game->ranking[i].level, game->ranking[i].survivalTime);
                    DrawText(rankText, screenWidth / 2 - MeasureText(rankText, 25) / 2, startY + i * lineHeight, 25, RAYWHITE);
                }
            }
            DrawText("Pressione ESC para voltar ao Menu", screenWidth / 2 - MeasureText("Pressione ESC para voltar ao Menu", 20) / 2, screenHeight - 50, 20, GRAY);
            break;
        }
    }
}

void UnloadGame(Game* game) {
    TraceLog(LOG_INFO, "GAME: Descarregando recursos do jogo.");

    if (game->player) {
        free(game->player);
        game->player = NULL;
    }
    if (game->map) {
        free(game->map);
        game->map = NULL;
    }
    for (int i = 0; i < MAX_ENEMIES_CAPACITY; i++) {
        if (game->enemies[i]) {
            free(game->enemies[i]);
            game->enemies[i] = NULL;
        }
    }
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (game->bombs[i]) {
            free(game->bombs[i]);
            game->bombs[i] = NULL;
        }
    }
}

void SpawnEnemy(Game* game) {
    Enemy* newEnemy = NULL;
    for (int i = 0; i < MAX_ENEMIES_CAPACITY; i++) {
        if (!game->enemies[i]->active) {
            newEnemy = game->enemies[i];
            break;
        }
    }

    if (newEnemy == NULL) {
        TraceLog(LOG_WARNING, "GAME: Não há inimigos inativos para spawnar!");
        return;
    }

    InitEnemy(newEnemy);

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float padding = 50.0f;

    Vector2 spawnPos;
    int side = GetRandomValue(0, 3);

    switch (side) {
        case 0:
            spawnPos = (Vector2){ (float)GetRandomValue(0, screenWidth), -padding };
            break;
        case 1:
            spawnPos = (Vector2){ (float)screenWidth + padding, (float)GetRandomValue(0, screenHeight) };
            break;
        case 2:
            spawnPos = (Vector2){ (float)GetRandomValue(0, screenWidth), (float)screenHeight + padding };
            break;
        case 3:
            spawnPos = (Vector2){ -padding, (float)GetRandomValue(0, screenHeight) };
            break;
    }
    newEnemy->position = spawnPos;

    newEnemy->active = true;
    TraceLog(LOG_INFO, "GAME: Inimigo spawnado na posição: (%.2f, %.2f)", newEnemy->position.x, newEnemy->position.y);
}

void LevelUpPlayer(Game* game) {
    game->player->currentXP -= game->xpRequiredForNextRound;
    game->currentRound++;

    game->xpRequiredForNextRound = (int)(game->xpRequiredForNextRound * 1.5f);
    if (game->xpRequiredForNextRound < 100) game->xpRequiredForNextRound = 100;

    game->activeMaxEnemies += 5;
    if (game->activeMaxEnemies > MAX_ENEMIES_CAPACITY) {
        game->activeMaxEnemies = MAX_ENEMIES_CAPACITY;
    }

    game->player->health += 20;
    if (game->player->health > game->player->maxHealth) game->player->health = game->player->maxHealth;
    game->player->bombCount = game->player->maxBombs;


    TraceLog(LOG_INFO, "GAME: Player subiu para o Round %d! XP necessario: %d. Max Inimigos Ativos: %d",
             game->currentRound, game->xpRequiredForNextRound, game->activeMaxEnemies);

    game->currentGameState = GAME_STATE_LEVEL_UP_OPTIONS;
    GenerateBuffOptions(game);
    game->currentBuffOption = BUFF_OPTION_1;
    game->menuInputTimer = 0.0f;

    TraceLog(LOG_INFO, "GAME: Transicionando para GAME_STATE_LEVEL_UP_OPTIONS.");
}

void GenerateBuffOptions(Game* game) {
    game->currentBuffScreen = GetRandomValue(0, 2);

    for (int i = 0; i < 3; ++i) {
        game->selectedBuffs[i] = buffScreens[game->currentBuffScreen][i];
    }
    TraceLog(LOG_INFO, "GAME: Geradas novas opcoes de buff (Tela %d):", game->currentBuffScreen);
    for (int i = 0; i < 3; ++i) {
        TraceLog(LOG_INFO, "  - Opcao %d: %s (Type: %d, Val: %.1f)", i + 1, game->selectedBuffs[i].debugText, game->selectedBuffs[i].type, game->selectedBuffs[i].value);
    }
}

// --- Funções de Ranking ---

void LoadRanking(Game* game, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        TraceLog(LOG_WARNING, "RANKING: Arquivo '%s' nao encontrado. Criando um novo.", filename);
        game->currentRankEntries = 0;
        return;
    }

    game->currentRankEntries = 0;
    while (game->currentRankEntries < MAX_RANK_ENTRIES &&
           fscanf(file, "%30s %d %f",
                  game->ranking[game->currentRankEntries].name,
                  &game->ranking[game->currentRankEntries].level,
                  &game->ranking[game->currentRankEntries].survivalTime) == 3) {
        game->currentRankEntries++;
    }
    fclose(file);
    TraceLog(LOG_INFO, "RANKING: Carregados %d entradas do ranking de '%s'.", game->currentRankEntries, filename);

    // Opcional: Reordenar após carregar, caso o arquivo esteja corrompido ou editado manualmente
    qsort(game->ranking, game->currentRankEntries, sizeof(RankEntry), CompareRankEntries);
}

void SaveRanking(const Game* game, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        TraceLog(LOG_ERROR, "RANKING: Nao foi possivel abrir/criar arquivo '%s' para escrita.", filename);
        return;
    }

    for (int i = 0; i < game->currentRankEntries; i++) {
        fprintf(file, "%s %d %.2f\n",
                game->ranking[i].name,
                game->ranking[i].level,
                game->ranking[i].survivalTime);
    }
    fclose(file);
    TraceLog(LOG_INFO, "RANKING: Salvos %d entradas do ranking em '%s'.", game->currentRankEntries, filename);
}

void AddScoreToRanking(Game* game, const char* playerName, int level, float survivalTime) {
    // Cria uma nova entrada temporária
    RankEntry newEntry;
    strncpy(newEntry.name, playerName, MAX_NAME_LENGTH);
    newEntry.name[MAX_NAME_LENGTH] = '\0'; // Garantir null-termination
    newEntry.level = level;
    newEntry.survivalTime = survivalTime;

    // Se o ranking ainda não estiver cheio, apenas adiciona e ordena
    if (game->currentRankEntries < MAX_RANK_ENTRIES) {
        game->ranking[game->currentRankEntries] = newEntry;
        game->currentRankEntries++;
    } else {
        // Ranking cheio, verifica se a nova pontuação é melhor que a pior
        RankEntry worstEntry = game->ranking[MAX_RANK_ENTRIES - 1];
        if (CompareRankEntries(&newEntry, &worstEntry) < 0) { // Se newEntry é melhor que worstEntry
            game->ranking[MAX_RANK_ENTRIES - 1] = newEntry; // Substitui a pior
            TraceLog(LOG_INFO, "RANKING: Nova pontuacao de %s (Nivel: %d, Tempo: %.2f) adicionada, substituindo a menor.", playerName, level, survivalTime);
        } else {
            TraceLog(LOG_INFO, "RANKING: Pontuacao de %s (Nivel: %d, Tempo: %.2f) nao e suficiente para entrar no ranking.", playerName, level, survivalTime);
            return; // Pontuação não entra no ranking
        }
    }

    // Ordena o ranking após a adição/substituição
    qsort(game->ranking, game->currentRankEntries, sizeof(RankEntry), CompareRankEntries);
    TraceLog(LOG_INFO, "RANKING: Ranking atualizado e ordenado. Total de entradas: %d", game->currentRankEntries);
}