#include "map.h"
#include "asset_manager.h" 
#include "raylib.h"

void InitMap(GameMap* map) {
    // Este mapa é virtualmente infinito pela forma como é desenhado,
    // então a inicialização da estrutura Map pode ser vazia por enquanto.
}

void DrawMap(const GameMap* map, const GameAssets* assets) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    Texture2D mapTexture = assets->infiniteMapSheet;

    // DEBUG VISUAL: Se a textura do mapa não carregou, desenha um quadrado CIANO
    if (mapTexture.id == 0) {
        TraceLog(LOG_ERROR, "DRAW_MAP_ERROR: Textura do mapa inválida! Verifique logs de carregamento.");
        DrawRectangle(0, 0, screenWidth, screenHeight, SKYBLUE); 
        return;
    }

    // Fator de escala para o mapa (3x para combinar com o player)
    float mapScale = 3.0f;

    // Largura e altura do tile do mapa APÓS a escala
    // Assumimos que a InfiniteMap.png é um tile único de TILE_SIZE x TILE_SIZE
    float scaledMapTileWidth = TILE_SIZE * mapScale;
    float scaledMapTileHeight = TILE_SIZE * mapScale;

    // Desenha o mapa repetidamente para cobrir toda a tela
    // Isso é útil para cenários "infinitos" com uma textura que se repete bem
    for (int y = 0; y < screenHeight; y += (int)scaledMapTileHeight) {
        for (int x = 0; x < screenWidth; x += (int)scaledMapTileWidth) {
            DrawTextureEx(mapTexture, (Vector2){(float)x, (float)y}, 0.0f, mapScale, WHITE);
        }
    }
}