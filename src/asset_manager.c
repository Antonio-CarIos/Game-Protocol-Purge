#include "asset_manager.h"
#include "raylib.h"
#include <stdio.h> 


void LoadGameAssets(GameAssets* assets) {
    TraceLog(LOG_INFO, "ASSET_MANAGER: Iniciando carregamento de assets...");

    char fullPath[256];

    // --- CARREGAMENTO DAS TEXTURAS DO PLAYER ---
    snprintf(fullPath, sizeof(fullPath), "assets/textures/player/RunX1.png");
    assets->playerRunSheet = LoadTexture(fullPath);
    if (assets->playerRunSheet.id == 0) {
        TraceLog(LOG_ERROR, "ASSET_MANAGER: FALHA ao carregar: %s", fullPath);
    } else {
        TraceLog(LOG_INFO, "ASSET_MANAGER: Carregado: %s (ID: %d)", fullPath, assets->playerRunSheet.id);
    }

    snprintf(fullPath, sizeof(fullPath), "assets/textures/player/idleX1.png");
    assets->playerIdleSheet = LoadTexture(fullPath);
    if (assets->playerIdleSheet.id == 0) {
        TraceLog(LOG_ERROR, "ASSET_MANAGER: FALHA ao carregar: %s", fullPath);
    } else {
        TraceLog(LOG_INFO, "ASSET_MANAGER: Carregado: %s (ID: %d)", fullPath, assets->playerIdleSheet.id);
    }

    snprintf(fullPath, sizeof(fullPath), "assets/textures/player/HitDea1.png");
    assets->playerHitDeathSheet = LoadTexture(fullPath);
    if (assets->playerHitDeathSheet.id == 0) {
        TraceLog(LOG_ERROR, "ASSET_MANAGER: FALHA ao carregar: %s", fullPath);
    } else {
        TraceLog(LOG_INFO, "ASSET_MANAGER: Carregado: %s (ID: %d)", fullPath, assets->playerHitDeathSheet.id);
    }

    // --- CARREGAMENTO DAS TEXTURAS DO INIMIGO ---
    snprintf(fullPath, sizeof(fullPath), "assets/textures/enemy/SnakeRunX1.png"); // <-- ALTERADO
    assets->snakeFullSheet = LoadTexture(fullPath); // Mantém o nome da variável, mas carrega a nova imagem
    if (assets->snakeFullSheet.id == 0) {
        TraceLog(LOG_ERROR, "ASSET_MANAGER: FALHA ao carregar: %s", fullPath);
    } else {
        TraceLog(LOG_INFO, "ASSET_MANAGER: Carregado: %s (ID: %d)", fullPath, assets->snakeFullSheet.id);
    }

    // --- CARREGAMENTO DAS TEXTURAS DE EFEITOS ---
    snprintf(fullPath, sizeof(fullPath), "assets/textures/effects/bomb_explosion.png");
    assets->bombExplosionSheet = LoadTexture(fullPath);
    if (assets->bombExplosionSheet.id == 0) {
        TraceLog(LOG_ERROR, "ASSET_MANAGER: FALHA ao carregar: %s", fullPath);
    } else {
        TraceLog(LOG_INFO, "ASSET_MANAGER: Carregado: %s (ID: %d)", fullPath, assets->bombExplosionSheet.id);
    }

    // --- CARREGAMENTO DAS TEXTURAS DO AMBIENTE (MAPA) ---
    snprintf(fullPath, sizeof(fullPath), "assets/textures/environment/InfiniteMap.png");
    assets->infiniteMapSheet = LoadTexture(fullPath);
    if (assets->infiniteMapSheet.id == 0) {
        TraceLog(LOG_ERROR, "ASSET_MANAGER: FALHA ao carregar: %s", fullPath);
    } else {
        TraceLog(LOG_INFO, "ASSET_MANAGER: Carregado: %s (ID: %d)", fullPath, assets->infiniteMapSheet.id);
    }

    TraceLog(LOG_INFO, "ASSET_MANAGER: Carregamento de assets concluído.");
}

void UnloadGameAssets(GameAssets* assets) {
    TraceLog(LOG_INFO, "ASSET_MANAGER: Descarregando assets...");
    UnloadTexture(assets->playerRunSheet);
    UnloadTexture(assets->playerIdleSheet);
    UnloadTexture(assets->playerHitDeathSheet);
    UnloadTexture(assets->snakeFullSheet);
    UnloadTexture(assets->bombExplosionSheet);
    UnloadTexture(assets->infiniteMapSheet);
    TraceLog(LOG_INFO, "ASSET_MANAGER: Assets descarregados.");
}