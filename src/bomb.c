#include "bomb.h"
#include "asset_manager.h" 
#include "raylib.h"

#include "raymath.h" 


void InitBomb(Bomb* bomb) {
    bomb->position = (Vector2){0, 0};
    bomb->active = false; // Começa inativa até ser ativada por evento do player
    
    bomb->activationTimer = 0.0f;
    bomb->activationDuration = 1.5f; // 1.5 segundos para a bomba "armar" e então explodir (reduzido)

    bomb->currentFrame = 0;
    bomb->frameTimer = 0.0f;
    bomb->frameSpeed = 0.08f; // Velocidade da animação de explosão (rápida)
    bomb->frameCount = 13;     // 13 frames da bomba de explosão
    bomb->hasExploded = false;
    bomb->damageApplied = false; // NENHUM DANO APLICADO AINDA

    // --- INICIALIZAÇÃO DE ATRIBUTOS PARA DANO ---
    bomb->damageRadius = TILE_SIZE * 3 * 1.5f; // Raio de dano da bomba (1.5x o tamanho do sprite escalado)
    bomb->damageAmount = 30;                 // Dano da bomba
}


void UpdateBomb(Bomb* bomb, float dt) {
    if (!bomb->active) return; 

    if (!bomb->hasExploded) {
       
        if (bomb->activationTimer < bomb->activationDuration) {
            bomb->activationTimer += dt;
            
            return; 
        }

        bomb->frameTimer += dt;
        if (bomb->frameTimer >= bomb->frameSpeed) {
            bomb->frameTimer = 0.0f;
            bomb->currentFrame++;
            if (bomb->currentFrame >= bomb->frameCount) {
                bomb->currentFrame = bomb->frameCount - 1; 
                bomb->hasExploded = true; 
                TraceLog(LOG_INFO, "BOMB: Bomba explodiu e terminou animacao.");
            }
        }
    } else {
      
        bomb->activationTimer += dt; 
        if (bomb->activationTimer >= (bomb->activationDuration + 0.5f)) { 
            bomb->active = false; 
            bomb->damageApplied = false; 
            bomb->activationTimer = 0.0f; 
            bomb->currentFrame = 0; 
            bomb->hasExploded = false; 
            TraceLog(LOG_INFO, "BOMB: Bomba desativada e pronta para reuso.");
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

    Rectangle sourceRec = {
        (float)bomb->currentFrame * TILE_SIZE,
        0.0f, 
        (float)TILE_SIZE,
        (float)TILE_SIZE
    };

    float scale = 3.0f;

    Rectangle destRec = {
        bomb->position.x,
        bomb->position.y,
        (float)TILE_SIZE * scale,
        (float)TILE_SIZE * scale
    };

    Vector2 origin = { (float)TILE_SIZE / 2 * scale, (float)TILE_SIZE / 2 * scale };

    Color drawColor = WHITE;
    if (!bomb->hasExploded && bomb->activationTimer < bomb->activationDuration) {
      
        if ((int)(bomb->activationTimer * 5) % 2 == 0) { 
            drawColor = (Color){255, 255, 255, 180}; 
        } else {
            drawColor = WHITE;
        }
    } else if (bomb->hasExploded) {
       
    }

    DrawTexturePro(currentSheet, sourceRec, destRec, origin, 0.0f, drawColor);

    #ifdef _DEBUG
        if (bomb->hasExploded) {
            DrawCircleLines((int)bomb->position.x, (int)bomb->position.y, bomb->damageRadius, BLUE);
        }
    #endif
}