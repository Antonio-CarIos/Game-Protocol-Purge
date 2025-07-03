// src/bomb.c
#include "bomb.h"
#include "asset_manager.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h> // Para TraceLog

void InitBomb(Bomb* bomb) {
    bomb->position = (Vector2){0, 0};
    bomb->active = false;
    bomb->activationTimer = 0.0f;
    bomb->activationDuration = 1.5f; // Tempo até a explosão

    bomb->currentFrame = 0;
    bomb->frameTimer = 0.0f;
    bomb->frameSpeed = 0.08f;
    bomb->frameCount = 13; // Total de frames na animação de explosão (0 a 12)
    bomb->hasExploded = false;
    bomb->damageApplied = false; // Flag para garantir que o dano é aplicado apenas uma vez por explosão

    bomb->damageRadius = TILE_SIZE * 3 * 1.5f; // Raio da área de dano da bomba
    bomb->damageAmount = 30; // Quantidade de dano da bomba

    // Definição dos retângulos de origem para cada frame da animação de explosão
    bomb->explosionSourceRecs[0] = (Rectangle){3.0f, 2.0f, 64.0f, 60.0f};
    bomb->explosionSourceRecs[1] = (Rectangle){66.0f, 2.0f, 64.0f, 60.0f};
    bomb->explosionSourceRecs[2] = (Rectangle){132.0f, 2.0f, 58.0f, 60.0f};
    bomb->explosionSourceRecs[3] = (Rectangle){196.0f, 2.0f, 59.0f, 60.0f};
    bomb->explosionSourceRecs[4] = (Rectangle){258.0f, 2.0f, 61.0f, 60.0f};
    bomb->explosionSourceRecs[5] = (Rectangle){322.0f, 2.0f, 59.0f, 60.0f};
    bomb->explosionSourceRecs[6] = (Rectangle){384.0f, 2.0f, 59.0f, 60.0f};
    bomb->explosionSourceRecs[7] = (Rectangle){448.0f, 2.0f, 63.0f, 60.0f};
    bomb->explosionSourceRecs[8] = (Rectangle){514.0f, 2.0f, 65.0f, 60.0f};
    bomb->explosionSourceRecs[9] = (Rectangle){582.0f, 2.0f, 57.0f, 60.0f};
    bomb->explosionSourceRecs[10] = (Rectangle){641.0f, 2.0f, 62.0f, 60.0f};
    bomb->explosionSourceRecs[11] = (Rectangle){704.0f, 2.0f, 64.0f, 60.0f};
    bomb->explosionSourceRecs[12] = (Rectangle){768.0f, 2.0f, 64.0f, 60.0f};

    TraceLog(LOG_DEBUG, "BOMB: Bomba inicializada. FrameCount: %d, FrameSpeed: %.2f", bomb->frameCount, bomb->frameSpeed);
}

void UpdateBomb(Bomb* bomb, float dt) {
    if (!bomb->active) return;

    if (!bomb->hasExploded) {
        // Lógica para a bomba antes de explodir (contagem regressiva)
        if (bomb->activationTimer < bomb->activationDuration) {
            bomb->activationTimer += dt;
            return; // Continua na contagem regressiva
        }
        // A bomba explodiu!
        bomb->hasExploded = true;
        bomb->currentFrame = 0;
        bomb->frameTimer = 0.0f;
        bomb->damageApplied = false; // Resetar para garantir que o dano é aplicado na animação
        TraceLog(LOG_INFO, "BOMB: Bomba explodiu em (%.2f, %.2f)!", bomb->position.x, bomb->position.y);
    } else {
        // Lógica para a animação de explosão
        bomb->frameTimer += dt;
        if (bomb->frameTimer >= bomb->frameSpeed) {
            bomb->frameTimer = 0.0f;
            bomb->currentFrame++;

            // NOVO: Condição para aplicar dano nos últimos frames
            // Por exemplo, últimos 3 frames (10, 11, 12 se total é 13 frames [0-12])
            int damageTriggerFrame = bomb->frameCount - 3;
            if (bomb->currentFrame >= damageTriggerFrame && !bomb->damageApplied) {
                // Setar damageApplied para true. game.c irá ler isso e aplicar o dano.
                // Isso garante que o dano é "ativado" no frame correto.
                bomb->damageApplied = true;
                TraceLog(LOG_DEBUG, "BOMB: Flag de dano da bomba ativada no frame %d", bomb->currentFrame);
            }

            if (bomb->currentFrame >= bomb->frameCount) {
                // A animação da explosão terminou
                bomb->active = false;          // Desativa a bomba
                bomb->damageApplied = false;   // Resetar para a próxima vez que a bomba for usada
                bomb->activationTimer = 0.0f;
                bomb->currentFrame = 0;
                bomb->hasExploded = false;
                TraceLog(LOG_INFO, "BOMB: Animação da explosao terminou. Bomba desativada.");
            }
        }
    }
}

void DrawBomb(const Bomb* bomb, const GameAssets* assets) {
    if (!bomb->active) return;

    Texture2D currentSheet = assets->bombExplosionSheet;

    if (currentSheet.id == 0) {
        TraceLog(LOG_ERROR, "DRAW_BOMB_ERROR: Textura da bomba inválida! Verifique logs de carregamento.");
        DrawRectangle(bomb->position.x - (TILE_SIZE * 3 / 2), bomb->position.y - (TILE_SIZE * 3 / 2), TILE_SIZE * 3, TILE_SIZE * 3, YELLOW);
        return;
    }

    if (bomb->hasExploded) {
        // Desenha a animação de explosão
        if (bomb->currentFrame < bomb->frameCount) {
            Rectangle sourceRec = bomb->explosionSourceRecs[bomb->currentFrame];
            // NOVO: Escala para a explosão, um pouco menor que o personagem (player scale é 3.0f)
            float scale = 2.5f; // Ajuste este valor conforme desejar
            Rectangle destRec = {
                bomb->position.x,
                bomb->position.y,
                sourceRec.width * scale,
                sourceRec.height * scale
            };
            Vector2 origin = { (sourceRec.width * scale) / 2, (sourceRec.height * scale) / 2 };
            DrawTexturePro(currentSheet, sourceRec, destRec, origin, 0.0f, WHITE);
        }
    } else {
        // REMOVIDO: O círculo branco de "sucção" antes da explosão.
        // A bomba é invisível ou pode ter um pequeno indicador, se quiser.
        // Por exemplo, um pequeno ponto cinza para indicar onde ela está:
        // DrawCircle(bomb->position.x, bomb->position.y, 5, DARKGRAY);
    }

    #ifdef _DEBUG
        // Desenha o círculo de dano (apenas para debug)
        if (bomb->hasExploded && bomb->active) {
            DrawCircleLines((int)bomb->position.x, (int)bomb->position.y, bomb->damageRadius, RED);
        }
    #endif
}