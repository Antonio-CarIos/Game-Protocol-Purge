// src/asset_manager.c
#include "asset_manager.h"
#include "raylib.h"
#include <stdio.h> // Para snprintf

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
    snprintf(fullPath, sizeof(fullPath), "assets/textures/enemy/SnakeRunX1.png");
    assets->snakeFullSheet = LoadTexture(fullPath);
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

    // CARREGAMENTO DA TEXTURA DA ANIMAÇÃO DA TELA DE INÍCIO
    snprintf(fullPath, sizeof(fullPath), "assets/textures/screens/Start.png");
    assets->startScreenAnimationSheet = LoadTexture(fullPath);
    if (assets->startScreenAnimationSheet.id == 0) {
        TraceLog(LOG_ERROR, "ASSET_MANAGER: FALHA ao carregar: %s", fullPath);
        TraceLog(LOG_ERROR, "ASSET_MANAGER: Verifique se 'assets/textures/screens/Start.png' existe.");
    } else {
        TraceLog(LOG_INFO, "ASSET_MANAGER: Carregado: %s (ID: %d)", fullPath, assets->startScreenAnimationSheet.id);
    }

    // CARREGAMENTO DA TEXTURA DA SPRITESHEET DE HUDS
    snprintf(fullPath, sizeof(fullPath), "assets/textures/hud/Huds.png");
    assets->hudsSheet = LoadTexture(fullPath);
    if (assets->hudsSheet.id == 0) {
        TraceLog(LOG_ERROR, "ASSET_MANAGER: FALHA ao carregar: %s", fullPath);
        TraceLog(LOG_ERROR, "ASSET_MANAGER: Verifique se 'assets/textures/hud/Huds.png' existe.");
    } else {
        TraceLog(LOG_INFO, "ASSET_MANAGER: Carregado: %s (ID: %d)", fullPath, assets->hudsSheet.id);
    }

    // NOVO: CARREGAMENTO DA TEXTURA DO MENU DE OPÇÕES (JÁ ESTAVA NO SEU CÓDIGO)
    snprintf(fullPath, sizeof(fullPath), "assets/textures/screens/Options.png");
    assets->optionsMenuSheet = LoadTexture(fullPath);
    if (assets->optionsMenuSheet.id == 0) {
        TraceLog(LOG_ERROR, "ASSET_MANAGER: FALHA ao carregar: %s", fullPath);
        TraceLog(LOG_ERROR, "ASSET_MANAGER: Verifique se 'assets/textures/screens/Options.png' existe.");
    } else {
        TraceLog(LOG_INFO, "ASSET_MANAGER: Carregado: %s (ID: %d)", fullPath, assets->optionsMenuSheet.id);
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
    UnloadTexture(assets->startScreenAnimationSheet);
    UnloadTexture(assets->hudsSheet);
    UnloadTexture(assets->optionsMenuSheet); // Descarrega a textura do menu de opções
    TraceLog(LOG_INFO, "ASSET_MANAGER: Assets descarregados.");
}