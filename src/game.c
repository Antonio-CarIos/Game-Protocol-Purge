// src/game.c
#include "game.h"
#include "raylib.h"
#include "player.h"
#include "enemy.h"
#include "bomb.h"
#include "map.h"
#include <stdlib.h>
#include <time.h>
#include "raymath.h"
#include <math.h> // Necessário para ceil()
#include <stdio.h> // Necessário para snprintf

// --- Funções Auxiliares Estáticas ---
// Array de definições das telas de buff (tipo, valor, texto de debug)
// A ordem das opções dentro de cada "tela" é importante para o mapeamento dos frames
static BuffEffect buffScreens[3][3] = {
    // Tela 0: +5% Dano, +1 Vida, +5% Velocidade
    { {2, 5.0f, "+5% Dmg"}, {1, 1.0f, "+1 Health"}, {0, 5.0f, "+5% Speed"} },
    // Tela 1: +5% Dano, +1 Vida, +1 Bomba
    { {2, 5.0f, "+5% Dmg"}, {1, 1.0f, "+1 Health"}, {3, 1.0f, "+1 Bomb"} },
    // Tela 2: +5% Dano, +2 Vida, +7% Velocidade
    { {2, 5.0f, "+5% Dmg"}, {1, 2.0f, "+2 Health"}, {0, 7.0f, "+7% Speed"} }
};

// Coordenadas dos frames no spritesheet Options.png para as telas de buff
// IMPORTANTE: Mantenha a ordem das telas (0, 1, 2) e das opções dentro de cada tela (0, 1, 2)
static Rectangle buffSourceRecs[3][3] = {
    // Tela 0
    {   {6.0f, 2.0f, 113.0f, 83.0f},   // Opção 0
        {121.0f, 2.0f, 113.0f, 83.0f}, // Opção 1
        {237.0f, 2.0f, 113.0f, 83.0f}  // Opção 2
    },
    // Tela 1
    {   {6.0f, 88.0f, 113.0f, 83.0f},   // Opção 0
        {120.0f, 88.0f, 113.0f, 83.0f}, // Opção 1
        {236.0f, 88.0f, 113.0f, 83.0f}  // Opção 2
    },
    // Tela 2
    {   {7.0f, 173.0f, 113.0f, 83.0f},   // Opção 0
        {120.0f, 173.0f, 113.0f, 83.0f}, // Opção 1
        {237.0f, 173.0f, 113.0f, 83.0f}  // Opção 2
    }
};


void InitGame(Game* game, Vector2 playerStartPos) {
    TraceLog(LOG_INFO, "GAME: Inicializando jogo...");
    InitPlayer(&game->player, playerStartPos);
    InitMap(&game->map); // Assumindo que InitMap inicializa a lógica do mapa

    for (int i = 0; i < MAX_ENEMIES_CAPACITY; i++) {
        InitEnemy(&game->enemies[i]); // Assumindo que InitEnemy inicializa inimigos
    }
    game->enemySpawnTimer = 0.0f;
    game->enemySpawnInterval = 3.0f;
    game->activeMaxEnemies = 5;

    for (int i = 0; i < MAX_BOMBS; i++) {
        InitBomb(&game->bombs[i]); // Assumindo que InitBomb inicializa bombas
    }

    game->currentRound = 1;
    game->xpRequiredForNextRound = 100;

    // Inicializa o estado do jogo e o timer
    game->currentGameState = GAME_STATE_START_MENU;
    game->gameTimer = 0.0f;

    // Inicializa o estado da animação da tela de início
    game->startScreenCurrentFrame = 0;
    game->startScreenFrameTimer = 0.0f;
    game->startScreenFrameSpeed = 0.1f;
    game->startScreenFrameCount = 16;

    // Inicializa o estado do menu de opções (PLAY, RANK, EXIT)
    game->currentMenuOption = MENU_OPTION_PLAY;
    game->menuInputDelay = 0.2f;
    game->menuInputTimer = 0.0f;

    // NOVO: Inicializa atributos do menu de buffs
    game->currentBuffScreen = 0; // Vai ser sorteado
    game->currentBuffOption = BUFF_OPTION_1; // Começa na primeira opção
    // selectedBuffs será preenchido em GenerateBuffOptions

    srand((unsigned int)time(NULL));
    TraceLog(LOG_INFO, "GAME: Jogo inicializado.");
}

void UpdateGame(Game* game, float dt) {
    game->menuInputTimer += dt; // Usa o mesmo timer para delays de input em menus

    // Lógica de atualização baseada no estado atual
    switch (game->currentGameState) {
        case GAME_STATE_START_MENU:
            // A atualização do start menu está em main.c. Nada aqui.
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
                            // Reinicializa o jogo para iniciar "limpo" (opcional, dependendo do design)
                            InitGame(game, (Vector2){ (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 });
                            game->currentGameState = GAME_STATE_PLAYING; // Força para PLAYING após InitGame
                            TraceLog(LOG_INFO, "MENU: Selecionado PLAY. Transicionando para GAME_STATE_PLAYING.");
                            break;
                        case MENU_OPTION_RANK:
                            TraceLog(LOG_INFO, "MENU: Selecionado RANK. (Funcionalidade a ser implementada).");
                            // TODO: Implementar tela de RANK
                            break;
                        case MENU_OPTION_EXIT:
                            TraceLog(LOG_INFO, "MENU: Selecionado EXIT. Fechando o jogo.");
                            CloseWindow();
                            break;
                    }
                }
            }
            break;

        case GAME_STATE_LEVEL_UP_OPTIONS: // Lógica do menu de buffs
            if (game->menuInputTimer >= game->menuInputDelay) {
                if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
                    game->currentBuffOption++;
                    if (game->currentBuffOption > BUFF_OPTION_3) { // 3 opções (0, 1, 2)
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
                    // Aplica o buff selecionado
                    BuffEffect chosenBuff = game->selectedBuffs[game->currentBuffOption];
                    PlayerApplyBuff(&game->player, chosenBuff.type, chosenBuff.value);
                    TraceLog(LOG_INFO, "LEVEL_UP_MENU: Buff aplicado: %s", chosenBuff.debugText);

                    // Retorna ao jogo
                    game->currentGameState = GAME_STATE_PLAYING;
                    TraceLog(LOG_INFO, "LEVEL_UP_MENU: Buff selecionado. Retornando ao jogo.");
                }
            }
            // Importante: Não atualizamos o jogo de fundo aqui, apenas o input do menu.
            break;

        case GAME_STATE_PLAYING:
            // Atualiza o timer do jogo
            game->gameTimer += dt;

            // Atualiza o jogador
            UpdatePlayer(&game->player, dt);

            // --- Lógica de soltar Bombas ---
            if (IsKeyPressed(KEY_E)) {
                if (game->player.bombCount > 0) {
                    Bomb* availableBomb = NULL;
                    for (int i = 0; i < MAX_BOMBS; i++) {
                        if (!game->bombs[i].active) {
                            availableBomb = &game->bombs[i];
                            break;
                        }
                    }

                    if (availableBomb != NULL) {
                        InitBomb(availableBomb);
                        availableBomb->position = game->player.position;
                        availableBomb->active = true;
                        game->player.bombCount--;
                        // Define o dano da bomba baseado no atributo do jogador
                        availableBomb->damageAmount = (int)(game->player.bombBaseDamage * game->player.baseDamageMultiplier);
                        TraceLog(LOG_INFO, "GAME: Bomba solta em (%.2f, %.2f). Bombas restantes: %d (Dano: %d)",
                                 availableBomb->position.x, availableBomb->position.y, game->player.bombCount, availableBomb->damageAmount);
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
                int currentActiveEnemies = 0;
                for (int i = 0; i < MAX_ENEMIES_CAPACITY; i++) {
                    if (game->enemies[i].active) {
                        currentActiveEnemies++;
                    }
                }

                if (currentActiveEnemies < game->activeMaxEnemies) {
                    SpawnEnemy(game);
                }
                game->enemySpawnTimer = 0.0f;
            }

            // Atualiza inimigos
            for (int i = 0; i < MAX_ENEMIES_CAPACITY; i++) {
                if (game->enemies[i].active) {
                    UpdateEnemy(&game->enemies[i], dt, &game->player);

                    // --- Colisão Inimigo vs. Inimigo ---
                    for (int j = i + 1; j < MAX_ENEMIES_CAPACITY; j++) {
                        if (game->enemies[j].active) {
                            if (CheckCollisionRecs(game->enemies[i].hitbox, game->enemies[j].hitbox)) {
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
                            int damage = 10;
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
                        for (int j = 0; j < MAX_ENEMIES_CAPACITY; j++) {
                            if (game->enemies[j].active) {
                                if (CheckCollisionCircleRec(game->bombs[i].position, game->bombs[i].damageRadius, game->enemies[j].hitbox)) {
                                    int xpGained = EnemyTakeDamage(&game->enemies[j], game->bombs[i].damageAmount);
                                    if (xpGained > 0) {
                                        game->player.currentXP += xpGained;
                                        TraceLog(LOG_INFO, "GAME: Player ganhou %d XP. Total: %d", xpGained, game->player.currentXP);
                                    }
                                }
                            }
                        }
                        game->bombs[i].damageApplied = true;
                        TraceLog(LOG_INFO, "GAME: Dano da bomba aplicado. Raio: %.2f", game->bombs[i].damageRadius);
                    }
                }
            }

            // Lógica de avanço de Round/Nível
            if (game->player.currentXP >= game->xpRequiredForNextRound) {
                LevelUpPlayer(game);
            }

            // Verifica condição de Game Over
            if (game->player.health <= 0) {
                game->currentGameState = GAME_STATE_DEATH;
                TraceLog(LOG_INFO, "GAME: Game Over! Player morreu!");
            }
            break;

        case GAME_STATE_DEATH:
            // Nada para atualizar aqui, a lógica de reinício está em main.c
            break;
    }
}


void DrawGame(const Game* game, const GameAssets* assets) {
    switch (game->currentGameState) {
        case GAME_STATE_START_MENU:
            // Já tratado em main.c ou pode ser movido para cá se preferir
            break;

        case GAME_STATE_OPTIONS_MENU: {
            // Coordenadas dos frames do menu principal (PLAY, RANK, EXIT)
            Rectangle menuSourceRecs[] = {
                {380.0f, 2.0f, 115.0f, 82.0f},   // PLAY
                {379.0f, 89.0f, 115.0f, 82.0f},  // RANK
                {380.0f, 175.0f, 115.0f, 82.0f}  // EXIT
            };

            int frameIndex = (int)game->currentMenuOption;
            // Validação simples
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

        case GAME_STATE_LEVEL_UP_OPTIONS: { // Desenho do menu de buffs com fundo escurecido
            // Primeiro, desenha o estado do jogo como se estivesse ativo, mas sem o HUD
            // Isso cria o "fundo" do jogo parado
            // Usamos infiniteMapSheet para o mapa, snakeFullSheet para inimigos e bombExplosionSheet para bombas
            DrawMap(&game->map, assets);
            DrawPlayer(&game->player, assets);
            for (int i = 0; i < MAX_ENEMIES_CAPACITY; i++) {
                DrawEnemy(&game->enemies[i], assets);
            }
            for (int i = 0; i < MAX_BOMBS; i++) {
                DrawBomb(&game->bombs[i], assets);
            }

            // Desenha um retângulo transparente preto sobre tudo para escurecer o fundo
            // A opacidade pode ser ajustada (e.g., 180 para um cinza escuro)
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f)); // 0.7f = 70% de opacidade

            // A tela de buff escolhida (0, 1 ou 2)
            int screenIndex = game->currentBuffScreen;
            // A opção selecionada dentro da tela (0, 1 ou 2)
            int optionIndex = (int)game->currentBuffOption;

            // Validação para evitar acesso fora dos limites
            if (screenIndex < 0 || screenIndex >= 3) screenIndex = 0;
            if (optionIndex < 0 || optionIndex >= 3) optionIndex = 0;

            Rectangle currentSourceRec = buffSourceRecs[screenIndex][optionIndex];

            float buffMenuScale = 3.0f; // Escala, pode ser diferente do menu principal
            float buffMenuWidth = currentSourceRec.width * buffMenuScale;
            float buffMenuHeight = currentSourceRec.height * buffMenuScale;
            float posX = GetScreenWidth() / 2 - buffMenuWidth / 2;
            float posY = GetScreenHeight() / 2 - buffMenuHeight / 2;

            Rectangle destRec = { posX, posY, buffMenuWidth, buffMenuHeight };

            // Desenha a textura do menu de opções de buff
            DrawTexturePro(assets->optionsMenuSheet, currentSourceRec, destRec, (Vector2){0,0}, 0.0f, WHITE);


            // DEBUG TEMPORÁRIO: Desenha os valores dos buffs abaixo do menu
            // Mude a cor para ser visível sobre o fundo escurecido
            float debugTextY = posY + buffMenuHeight + 20;
            for (int i = 0; i < 3; ++i) {
                char text[64];
                BuffEffect buff = game->selectedBuffs[i];
                const char* buffName = "";

                switch(buff.type) {
                    case 0: buffName = "Speed"; break;
                    case 1: buffName = "Health"; break;
                    case 2: buffName = "Damage"; break;
                    case 3: buffName = "BombCap"; break;
                }
                snprintf(text, sizeof(text), "Opcao %d: %s (Type: %d, Val: %.1f)", i + 1, buffName, buff.type, buff.value);
                DrawText(text, posX, debugTextY + (i * 25), 20, LIGHTGRAY); // Usando LIGHTGRAY para visibilidade
            }
            break;
        }

        case GAME_STATE_PLAYING:
            // Desenhar o mapa primeiro
            DrawMap(&game->map, assets);

            // Desenhar o player
            DrawPlayer(&game->player, assets);

            // Desenhar inimigos ativos
            for (int i = 0; i < MAX_ENEMIES_CAPACITY; i++) {
                DrawEnemy(&game->enemies[i], assets);
            }

            // Desenhar bombas ativas
            for (int i = 0; i < MAX_BOMBS; i++) {
                DrawBomb(&game->bombs[i], assets);
            }

            // --- HUD (informações do jogo) ---
            float hudScale = 3.0f; // Escala global para os elementos do HUD em spritesheet

            // O HUD precisa de assets->hudsSheet.id != 0 para desenhar
            if (assets->hudsSheet.id != 0) {
                // --- Desenha a barra de vida ---
                Rectangle healthSourceRec = { 0 };
                Vector2 healthDrawPos = { 150, 40 }; // Ajuste conforme o layout final desejado

                if (game->player.health >= 81) {
                    healthSourceRec = (Rectangle){ 9.0f, 74.0f, 41.0f, 12.0f }; // 5 barrinhas
                } else if (game->player.health >= 61) {
                    healthSourceRec = (Rectangle){ 9.0f, 86.0f, 41.0f, 12.0f }; // 4 barrinhas
                } else if (game->player.health >= 41) {
                    healthSourceRec = (Rectangle){ 9.0f, 98.0f, 41.0f, 12.0f }; // 3 barrinhas
                } else if (game->player.health >= 21) {
                    healthSourceRec = (Rectangle){ 9.0f, 110.0f, 41.0f, 12.0f }; // 2 barrinhas
                } else if (game->player.health >= 1) {
                    healthSourceRec = (Rectangle){ 9.0f, 122.0f, 41.0f, 12.0f }; // 1 barrinha
                } else { // player->health <= 0
                    healthSourceRec = (Rectangle){ 9.0f, 134.0f, 41.0f, 12.0f }; // 0 barrinhas
                }

                Rectangle healthDestRec = { healthDrawPos.x, healthDrawPos.y, healthSourceRec.width * hudScale, healthSourceRec.height * hudScale };
                DrawTexturePro(assets->hudsSheet, healthSourceRec, healthDestRec, (Vector2){0,0}, 0.0f, WHITE);


                // --- Desenha o indicador de Bombas ---
                Rectangle bombSourceRec = { 0 };
                // Ajustado para ficar à direita da barra de vida + um pouco de espaçamento
                Vector2 bombDrawPos = { healthDrawPos.x + (healthSourceRec.width * hudScale) + 20, healthDrawPos.y };

                // Mapeia o número de bombas para o frame correto (frame 1 = 5 bombas, frame 6 = 0 bombas)
                // Assumindo player.maxBombs = 5
                int bombFrameIndex = 5 - game->player.bombCount;
                if (bombFrameIndex < 0) bombFrameIndex = 0;
                if (bombFrameIndex > 5) bombFrameIndex = 5;

                // Ys para bomba: 87, 98, 109, 120, 131, 142 (incremento de 11)
                bombSourceRec = (Rectangle){ 114.0f, 87.0f + (bombFrameIndex * 11.0f), 42.0f, 10.0f };

                Rectangle bombDestRec = { bombDrawPos.x, bombDrawPos.y, bombSourceRec.width * hudScale, bombSourceRec.height * hudScale };
                DrawTexturePro(assets->hudsSheet, bombSourceRec, bombDestRec, (Vector2){0,0}, 0.0f, WHITE);


                // --- Desenha a barra de XP ---
                Rectangle xpSourceRec = { 0 };
                // Posiciona a barra de XP mais ao centro horizontalmente, um pouco abaixo.
                Vector2 xpDrawPos = { GetScreenWidth() / 2 - (135 * hudScale / 2), 120 };

                int xpFrameIndex = 1; // Default para frame 1 (vazio)
                if (game->xpRequiredForNextRound > 0) {
                    float xpPercentage = (float)game->player.currentXP / game->xpRequiredForNextRound;
                    // Mapeia para 0-22 segmentos
                    int filledSegments = (int)(xpPercentage * 22.0f);
                    if (filledSegments < 0) filledSegments = 0;
                    if (filledSegments > 22) filledSegments = 22; // Garante que não exceda o número máximo de segmentos

                    xpFrameIndex = filledSegments + 1; // Converte para índice de frame (1 a 23)
                }

                // Coordenadas Y para XP: 122, 130, 138, ..., 303 (incremento de 8)
                xpSourceRec = (Rectangle){ 192.0f, 122.0f + (xpFrameIndex - 1) * 8.0f, 135.0f, 6.0f };

                Rectangle xpDestRec = { xpDrawPos.x, xpDrawPos.y, xpSourceRec.width * hudScale, xpSourceRec.height * hudScale };
                DrawTexturePro(assets->hudsSheet, xpSourceRec, xpDestRec, (Vector2){0,0}, 0.0f, WHITE);

            } else {
                // Fallback se a textura do HUD não carregou
                DrawText(TextFormat("Health: %d / %d", game->player.health, game->player.maxHealth), 10, 40, 20, LIME);
                DrawText(TextFormat("Bombs: %d / %d", game->player.bombCount, game->player.maxBombs), 10, 70, 20, ORANGE);
                DrawText(TextFormat("XP: %d / %d", game->player.currentXP, game->xpRequiredForNextRound), 10, 130, 20, GOLD);
            }


            // Indicadores que ainda são texto
            DrawText(TextFormat("Round: %d", game->currentRound), GetScreenWidth() / 2 - MeasureText(TextFormat("Round: %d", game->currentRound), 20) / 2, 90, 20, RAYWHITE);
            DrawText(TextFormat("TIME: %.2f", game->gameTimer), 10, 160, 20, YELLOW);

            // DEBUG TEMPORÁRIO: Exibe atributos do jogador na tela
            DrawText(TextFormat("DEBUG - Speed: %.2f", game->player.speed), 10, GetScreenHeight() - 70, 20, PURPLE);
            DrawText(TextFormat("DEBUG - Base Damage Multiplier: %.2f", game->player.baseDamageMultiplier), 10, GetScreenHeight() - 50, 20, PURPLE);
            DrawText(TextFormat("DEBUG - Bomb Base Damage: %d", game->player.bombBaseDamage), 10, GetScreenHeight() - 30, 20, PURPLE);
            break;

        case GAME_STATE_DEATH:
            // Já tratado em main.c
            break;
    }
}

void UnloadGame(Game* game) {
    TraceLog(LOG_INFO, "GAME: Descarregando recursos do jogo (além dos assets globais).");
}

// --- Funções Auxiliares de Jogo ---

void SpawnEnemy(Game* game) {
    Enemy* newEnemy = NULL;
    for (int i = 0; i < MAX_ENEMIES_CAPACITY; i++) {
        if (!game->enemies[i].active) {
            newEnemy = &game->enemies[i];
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

    // A posição do player é relativa ao centro da tela de colisão.
    // O ideal é spawnar o inimigo FORA da tela visível.
    // Vamos escolher um ponto aleatório fora dos limites da tela para o spawn.
    Vector2 spawnPos;
    int side = GetRandomValue(0, 3); // 0: top, 1: right, 2: bottom, 3: left

    switch (side) {
        case 0: // Top
            spawnPos = (Vector2){ (float)GetRandomValue(0, screenWidth), -padding };
            break;
        case 1: // Right
            spawnPos = (Vector2){ (float)screenWidth + padding, (float)GetRandomValue(0, screenHeight) };
            break;
        case 2: // Bottom
            spawnPos = (Vector2){ (float)GetRandomValue(0, screenWidth), (float)screenHeight + padding };
            break;
        case 3: // Left
            spawnPos = (Vector2){ -padding, (float)GetRandomValue(0, screenHeight) };
            break;
    }
    newEnemy->position = spawnPos;

    newEnemy->active = true;
    TraceLog(LOG_INFO, "GAME: Inimigo spawnado na posição: (%.2f, %.2f)", newEnemy->position.x, newEnemy->position.y);
}

void LevelUpPlayer(Game* game) {
    game->player.currentXP -= game->xpRequiredForNextRound;
    game->currentRound++;

    game->xpRequiredForNextRound = (int)(game->xpRequiredForNextRound * 1.5f);
    if (game->xpRequiredForNextRound < 100) game->xpRequiredForNextRound = 100;

    game->activeMaxEnemies += 5;
    if (game->activeMaxEnemies > MAX_ENEMIES_CAPACITY) {
        game->activeMaxEnemies = MAX_ENEMIES_CAPACITY;
    }

    // NOVO: Cura e garante que não excede a vida máxima
    game->player.health += 20;
    if (game->player.health > game->player.maxHealth) game->player.health = game->player.maxHealth;
    game->player.bombCount = game->player.maxBombs; // Opcional: enche as bombas ao subir de nível


    TraceLog(LOG_INFO, "GAME: Player subiu para o Round %d! XP necessario: %d. Max Inimigos Ativos: %d",
             game->currentRound, game->xpRequiredForNextRound, game->activeMaxEnemies);

    // MUDANÇA CRÍTICA: Transiciona para o estado de Level Up Options
    game->currentGameState = GAME_STATE_LEVEL_UP_OPTIONS;
    GenerateBuffOptions(game); // Gera as 3 opções de buff para o jogador escolher
    game->currentBuffOption = BUFF_OPTION_1; // Reinicia a seleção para a primeira opção
    game->menuInputTimer = 0.0f; // Reinicia o timer de input para evitar seleção automática

    TraceLog(LOG_INFO, "GAME: Transicionando para GAME_STATE_LEVEL_UP_OPTIONS.");
}

// NOVO: Implementação da função GenerateBuffOptions
void GenerateBuffOptions(Game* game) {
    // Sorteia uma das 3 telas de buff disponíveis
    game->currentBuffScreen = GetRandomValue(0, 2); // 0, 1 ou 2

    // Preenche as opções de buff do jogo com as da tela sorteada
    for (int i = 0; i < 3; ++i) {
        game->selectedBuffs[i] = buffScreens[game->currentBuffScreen][i];
    }
    TraceLog(LOG_INFO, "GAME: Geradas novas opcoes de buff (Tela %d):", game->currentBuffScreen);
    for (int i = 0; i < 3; ++i) {
        TraceLog(LOG_INFO, "  - Opcao %d: %s (Tipo: %d, Val: %.1f)", i + 1, game->selectedBuffs[i].debugText, game->selectedBuffs[i].type, game->selectedBuffs[i].value);
    }
}