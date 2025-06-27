#include "raylib.h"
#include "asset_manager.h" 
#include "game.h"         


// Variáveis globais
GameAssets g_gameAssets; // Instância global de todos os assets carregados
Game g_myGame;           // Instância global do jogo
// GameMap g_gameMap;     

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    SetTraceLogLevel(LOG_DEBUG); 

    InitWindow(screenWidth, screenHeight, "Meu Vampire Survivors Simplificado");
    SetTargetFPS(60); // Define o limite de frames por segundo

    LoadGameAssets(&g_gameAssets); // Carrega todos os assets do jogo

    // Inicializa o jogo 
    InitGame(&g_myGame, (Vector2){ (float)screenWidth / 2, (float)screenHeight / 2 });

    // Loop principal do jogo
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime(); // Tempo desde o último frame

        // 1. Atualização da lógica do jogo
        UpdateGame(&g_myGame, deltaTime);

        // 2. Desenho (renderização)
        BeginDrawing();
            ClearBackground(BLACK);

            // Desenha todo o estado do jogo (mapa, player, inimigos, bombas)
            DrawGame(&g_myGame, &g_gameAssets); 

            DrawFPS(10, 10); // Exibe o FPS

        EndDrawing();
    }

    // Descarrega os assets e libera recursos do jogo
    UnloadGameAssets(&g_gameAssets);
    UnloadGame(&g_myGame); 

    CloseWindow();
    return 0;
}