// src/enemy.c
#include "enemy.h"
#include "player.h" // <<--- ADICIONE ESTA LINHA para que a definicao completa de Player seja conhecida
#include "asset_manager.h"
#include "raymath.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h> // Para TraceLog

static int GetEnemyFrameCountForState(EnemyAnimationState state) {
    switch (state) {
        case ENEMY_STATE_RUN: return 7;
        case ENEMY_STATE_DEATH: return 4;
        default: return 1;
    }
}

static int GetEnemyFrameRowForDirection(EnemyDirection dir) {
    return (int)dir;
}

void InitEnemy(Enemy* enemy) {
    enemy->position = (Vector2){0, 0};
    enemy->targetPosition = (Vector2){0, 0};
    enemy->speed = 80.0f;
    enemy->active = false;

    enemy->currentState = ENEMY_STATE_RUN;
    enemy->currentDirection = ENEMY_DIR_DOWN;

    enemy->currentFrame = 0;
    enemy->frameTimer = 0.0f;
    enemy->frameSpeed = 0.15f;

    enemy->hitbox = (Rectangle){0, 0, 0, 0};

    enemy->health = 50;
    enemy->isInvulnerable = false;
    enemy->invulnerableTimer = 0.0f;
    enemy->invulnerableDuration = 0.5f;

    enemy->xpValue = 5;
}

void UpdateEnemy(Enemy* enemy, float dt, const Player* player) {
    if (!enemy->active) return;

    // Acessando player->position agora funciona porque player.h foi incluído,
    // o que indiretamente puxa a definição completa de Player via types.h
    enemy->targetPosition = player->position;

    Vector2 direction = Vector2Subtract(enemy->targetPosition, enemy->position);

    if (fabs(direction.x) > fabs(direction.y)) {
        if (direction.x > 0) enemy->currentDirection = ENEMY_DIR_RIGHT;
        else enemy->currentDirection = ENEMY_DIR_LEFT;
    } else {
        if (direction.y > 0) enemy->currentDirection = ENEMY_DIR_DOWN;
        else enemy->currentDirection = ENEMY_DIR_UP;
    }

    direction = Vector2Normalize(direction);

    enemy->position.x += direction.x * enemy->speed * dt;
    enemy->position.y += direction.y * enemy->speed * dt;

    int totalFramesForCurrentAnimation = GetEnemyFrameCountForState(enemy->currentState);

    enemy->frameTimer += dt;
    if (enemy->frameTimer >= enemy->frameSpeed) {
        enemy->frameTimer = 0.0f;
        enemy->currentFrame++;
        if (enemy->currentFrame >= totalFramesForCurrentAnimation) {
            enemy->currentFrame = 0;
        }
    }

    if (enemy->isInvulnerable) {
        enemy->invulnerableTimer += dt;
        if (enemy->invulnerableTimer >= enemy->invulnerableDuration) {
            enemy->isInvulnerable = false;
            enemy->invulnerableTimer = 0.0f;
        }
    }

    enemy->hitbox = GetEnemyHitbox(enemy);
}

void DrawEnemy(const Enemy* enemy, const GameAssets* assets) {
    if (!enemy->active) return;

    Texture2D currentSheet = assets->snakeFullSheet;

    if (currentSheet.id == 0) {
        TraceLog(LOG_ERROR, "DRAW_ENEMY_ERROR: Textura da cobra inválida! Verifique logs de carregamento.");
        DrawRectangle(enemy->position.x - (TILE_SIZE * 3 / 2), enemy->position.y - (TILE_SIZE * 3 / 2), TILE_SIZE * 3, TILE_SIZE * 3, RED);
        return;
    }

    int sourceRow = GetEnemyFrameRowForDirection(enemy->currentDirection);
    int frameOffset = 0;

    Rectangle sourceRec = {
        (float)(enemy->currentFrame + frameOffset) * TILE_SIZE,
        (float)sourceRow * TILE_SIZE,
        (float)TILE_SIZE,
        (float)TILE_SIZE
    };

    float scale = 3.0f;

    Rectangle destRec = {
        enemy->position.x,
        enemy->position.y,
        (float)TILE_SIZE * scale,
        (float)TILE_SIZE * scale
    };

    Vector2 origin = { (float)TILE_SIZE / 2 * scale, (float)TILE_SIZE / 2 * scale };

    Color drawColor = WHITE;
    if (enemy->isInvulnerable) {
        if ((int)(enemy->invulnerableTimer * 10) % 2 == 0) {
            drawColor = (Color){255, 255, 255, 150};
        }
    }

    DrawTexturePro(currentSheet, sourceRec, destRec, origin, 0.0f, drawColor);

    #ifdef _DEBUG
        DrawRectangleLinesEx(enemy->hitbox, 1, RED);
        DrawText(TextFormat("%d", enemy->health), (int)enemy->position.x - 10, (int)enemy->position.y - 40, 10, WHITE);
    #endif
}

Rectangle GetEnemyHitbox(const Enemy* enemy) {
    float hitboxWidth = TILE_SIZE * 3 * 0.6f;
    float hitboxHeight = TILE_SIZE * 3 * 0.5f;

    return (Rectangle){
        enemy->position.x - hitboxWidth / 2,
        enemy->position.y - hitboxHeight / 2,
        hitboxWidth,
        hitboxHeight
    };
}

int EnemyTakeDamage(Enemy* enemy, int damage) {
    if (enemy->isInvulnerable || !enemy->active) return 0;

    enemy->health -= damage;
    TraceLog(LOG_INFO, "ENEMY: Inimigo recebeu %d de dano! Vida atual: %d", damage, enemy->health);

    enemy->isInvulnerable = true;
    enemy->invulnerableTimer = 0.0f;

    if (enemy->health <= 0) {
        enemy->active = false;
        TraceLog(LOG_INFO, "ENEMY: Inimigo derrotado!");
        return enemy->xpValue;
    }
    return 0;
}