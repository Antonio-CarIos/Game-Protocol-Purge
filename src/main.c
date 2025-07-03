#include "raylib.h"
#include "asset_manager.h"
#include "game.h"


// Variáveis globais
GameAssets g_gameAssets;
Game g_myGame;


int main() {
    const int screenWidth = 1000;
    const int screenHeight = 600;

    SetTraceLogLevel(LOG_DEBUG);

    InitWindow(screenWidth, screenHeight, "Meu Vampire Survivors Simplificado");
    SetTargetFPS(60);

    LoadGameAssets(&g_gameAssets);

    g_myGame.startScreenTexture = g_gameAssets.startScreenAnimationSheet;
    // NOVO: Atribui a textura do menu de opções
    // (A linha abaixo é redundante, pois a textura já está em g_gameAssets.optionsMenuSheet)
    // g_myGame.optionsMenuTexture = g_gameAssets.optionsMenuSheet;


    // Inicializa o jogo (agora começa no START_MENU)
    InitGame(&g_myGame, (Vector2){ (float)screenWidth / 2, (float)screenHeight / 2 });

    // Loop principal do jogo
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Atualização da lógica do jogo baseada no estado atual
        switch (g_myGame.currentGameState) {
            case GAME_STATE_START_MENU:
                // Atualiza a animação da tela de início
                g_myGame.startScreenFrameTimer += deltaTime;
                if (g_myGame.startScreenFrameTimer >= g_myGame.startScreenFrameSpeed) {
                    g_myGame.startScreenFrameTimer = 0.0f;
                    g_myGame.startScreenCurrentFrame++;
                    if (g_myGame.startScreenCurrentFrame >= g_myGame.startScreenFrameCount) {
                        g_myGame.startScreenCurrentFrame = 0; // Loop da animação
                    }
                }
                // Transição para o menu de opções ao pressionar ESPAÇO ou ENTER
                if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                    g_myGame.currentGameState = GAME_STATE_OPTIONS_MENU; // MUDANÇA AQUI
                    TraceLog(LOG_INFO, "MAIN: Transicionando para GAME_STATE_OPTIONS_MENU ao pressionar SPACE ou ENTER.");
                }
                break;
            case GAME_STATE_OPTIONS_MENU: // Lógica do menu de opções
                // A lógica de input e transição será em UpdateGame
                UpdateGame(&g_myGame, deltaTime);
                break;
            case GAME_STATE_LEVEL_UP_OPTIONS: // NOVO: Lógica de atualização para o menu de buffs
                UpdateGame(&g_myGame, deltaTime); // Chama a função de atualização para o menu de buffs
                break;
            case GAME_STATE_PLAYING:
                UpdateGame(&g_myGame, deltaTime); // Chama a função de atualização principal do jogo
                break;
            case GAME_STATE_DEATH:
                if (IsKeyPressed(KEY_R)) {
                    InitGame(&g_myGame, (Vector2){ (float)screenWidth / 2, (float)screenHeight / 2 });
                    g_myGame.currentGameState = GAME_STATE_PLAYING;
                    TraceLog(LOG_INFO, "MAIN: Jogo Reiniciado.");
                }
                break;
        }

        // Desenho (renderização)
        BeginDrawing();
            ClearBackground(BLACK);

            // Desenho baseado no estado atual
            switch (g_myGame.currentGameState) {
                case GAME_STATE_START_MENU:
                    // Desenha a animação da tela de início
                    if (g_myGame.startScreenTexture.id != 0) {
                        Rectangle sourceRec = {
                            (float)g_myGame.startScreenCurrentFrame * screenWidth,
                            0.0f,
                            (float)screenWidth,
                            (float)screenHeight
                        };
                        DrawTexturePro(g_myGame.startScreenTexture, sourceRec,
                                       (Rectangle){0, 0, (float)screenWidth, (float)screenHeight},
                                       (Vector2){0, 0}, 0.0f, WHITE);
                    } else {
                        DrawText("START MENU (Pressione ESPAÇO ou ENTER para iniciar)", screenWidth / 2 - MeasureText("START MENU (Pressione ESPAÇO ou ENTER para iniciar)", 30) / 2, screenHeight / 2 - 15, 30, RAYWHITE);
                    }
                    break;
                case GAME_STATE_OPTIONS_MENU: // Desenho do menu de opções
                    DrawGame(&g_myGame, &g_gameAssets); // Reutiliza DrawGame para desenhar o menu
                    break;
                case GAME_STATE_LEVEL_UP_OPTIONS: // <--- AQUI ESTÁ A CHAVE!
                    DrawGame(&g_myGame, &g_gameAssets); // Desenha o menu de level up
                    break;
                case GAME_STATE_PLAYING:
                    DrawGame(&g_myGame, &g_gameAssets);
                    break;
                case GAME_STATE_DEATH:
                    DrawText("VOCÊ MORREU!", screenWidth / 2 - MeasureText("VOCÊ MORREU!", 40) / 2, screenHeight / 2 - 40, 40, RED);
                    DrawText(TextFormat("Tempo de Sobrevivência: %.2f segundos", g_myGame.gameTimer),
                                     screenWidth / 2 - MeasureText(TextFormat("Tempo de Sobrevivência: %.2f segundos", g_myGame.gameTimer), 20) / 2,
                                     screenHeight / 2 + 20, 20, RAYWHITE);
                    DrawText("Pressione R para Reiniciar", screenWidth / 2 - MeasureText("Pressione R para Reiniciar", 20) / 2, screenHeight - 50, 20, GRAY);
                    break;
            }
            DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadGameAssets(&g_gameAssets);
    UnloadGame(&g_myGame);

    CloseWindow();
    return 0;
}