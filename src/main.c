// src/main.c
#include "raylib.h"
#include "asset_manager.h"
#include "game.h" // Apenas inclui game.h, que por sua vez inclui types.h e asset_manager.h
#include "player.h" // Inclui player.h para que Player seja conhecido
#include "enemy.h"  // Inclui enemy.h para que Enemy seja conhecido
#include "bomb.h"   // Inclui bomb.h para que Bomb seja conhecido
#include "map.h"    // Inclui map.h para que GameMap seja conhecido


// Variáveis globais
GameAssets g_gameAssets;
Game g_myGame; // Game é uma struct normal, mas seus membros são ponteiros

int main() {
    const int screenWidth = 1000;
    const int screenHeight = 600;

    SetTraceLogLevel(LOG_DEBUG);

    InitWindow(screenWidth, screenHeight, "Meu Vampire Survivors Simplificado");
    SetTargetFPS(60);

    LoadGameAssets(&g_gameAssets);

    // No main, você passa o endereço de g_myGame
    InitGame(&g_myGame, (Vector2){ (float)screenWidth / 2, (float)screenHeight / 2 });

    // A textura da tela de início é copiada para a struct Game
    g_myGame.startScreenTexture = g_gameAssets.startScreenAnimationSheet;

    // Loop principal do jogo
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        switch (g_myGame.currentGameState) {
            case GAME_STATE_START_MENU:
                // A lógica de atualização do start menu já está em game.c
                UpdateGame(&g_myGame, deltaTime); // Chama para processar input e animacao
                break;
            case GAME_STATE_OPTIONS_MENU:
            case GAME_STATE_NAME_INPUT: // NOVO: Adicionar este estado
            case GAME_STATE_LEVEL_UP_OPTIONS:
            case GAME_STATE_PLAYING:
            case GAME_STATE_RANKING_VIEW: // NOVO: Adicionar este estado
                UpdateGame(&g_myGame, deltaTime);
                break;
            case GAME_STATE_DEATH:
                // Ao morrer, o ranking já foi salvo em UpdateGame
                if (IsKeyPressed(KEY_R)) {
                    UnloadGame(&g_myGame); // Desaloca recursos atuais
                    // Reinicializa o jogo para uma nova partida
                    InitGame(&g_myGame, (Vector2){ (float)screenWidth / 2, (float)screenHeight / 2 });
                    g_myGame.startScreenTexture = g_gameAssets.startScreenAnimationSheet; // Reatribui a textura da tela inicial
                    g_myGame.currentGameState = GAME_STATE_PLAYING; // Inicia um novo jogo diretamente
                    TraceLog(LOG_INFO, "MAIN: Jogo Reiniciado.");
                }
                break;
        }

        BeginDrawing();
            ClearBackground(BLACK);

            // A lógica de desenho é toda encapsulada em DrawGame
            DrawGame(&g_myGame, &g_gameAssets);

            DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadGameAssets(&g_gameAssets);
    UnloadGame(&g_myGame); // Garante que a memória é liberada no final

    CloseWindow();
    return 0;
}