// src/player.c
#include "player.h"
#include "asset_manager.h"
#include "raylib.h"
#include "raymath.h"
#include <math.h> // Para fmaxf, fminf
#include <stdio.h> // Para TraceLog
#include <string.h> // Para strcpy, memset

static int GetFrameCountForState(PlayerAnimationState state) {
    switch (state) {
        case PLAYER_STATE_IDLE: return 8;
        case PLAYER_STATE_RUN: return 8;
        case PLAYER_STATE_HIT_DEATH: return 4;
        default: return 1;
    }
}

static int GetFrameRowForDirection(PlayerDirection dir) {
    return (int)dir;
}

void InitPlayer(Player* player, Vector2 startPos) {
    player->position = startPos;
    player->speed = 100.0f;
    player->currentState = PLAYER_STATE_IDLE;
    player->currentDirection = PLAYER_DIR_DOWN;

    player->currentFrame = 0;
    player->frameTimer = 0.0f;
    player->frameSpeed = 0.1f;

    player->health = 100;
    player->maxHealth = 100;
    player->isInvulnerable = false;
    player->invulnerableTimer = 0.0f;
    player->invulnerableDuration = 1.5f;

    player->hitbox = (Rectangle){0, 0, 0, 0};

    player->escapeSpeedBonus = 70.0f;
    player->escapeSpeedTimer = 0.0f;
    player->escapeSpeedDuration = 1.0f;

    player->maxBombs = 5;
    player->bombCount = player->maxBombs;
    player->bombRechargeTimer = 0.0f;
    player->bombRechargeInterval = 5.0f;
    player->bombsPerClick = 1;

    player->currentXP = 0;

    player->baseDamageMultiplier = 1.0f;
    player->bombBaseDamage = 50;

    // NOVO: Inicializa o nome do jogador
    memset(player->name, 0, sizeof(player->name)); // Limpa o buffer do nome
    strcpy(player->name, "Player"); // Nome padrao inicial
}

void UpdatePlayer(Player* player, float dt) {
    Vector2 input = {0.0f, 0.0f};

    if (IsKeyDown(KEY_W)) input.y -= 1.0f;
    if (IsKeyDown(KEY_S)) input.y += 1.0f;
    if (IsKeyDown(KEY_A)) input.x -= 1.0f;
    if (IsKeyDown(KEY_D)) input.x += 1.0f;

    if (Vector2Length(input) > 0) {
        input = Vector2Normalize(input);
        player->currentState = PLAYER_STATE_RUN;
    } else {
        player->currentState = PLAYER_STATE_IDLE;
    }

    if (fabs(input.x) > fabs(input.y)) {
        if (input.x > 0) player->currentDirection = PLAYER_DIR_RIGHT;
        else if (input.x < 0) player->currentDirection = PLAYER_DIR_LEFT;
    }
    else if (fabs(input.y) > 0) {
        if (input.y > 0) player->currentDirection = PLAYER_DIR_DOWN;
        else if (input.y < 0) player->currentDirection = PLAYER_DIR_UP;
    }

    float currentSpeed = player->speed;
    if (player->escapeSpeedTimer > 0) {
        currentSpeed += player->escapeSpeedBonus;
        player->escapeSpeedTimer -= dt;
        if (player->escapeSpeedTimer <= 0) {
            player->escapeSpeedTimer = 0.0f;
            TraceLog(LOG_INFO, "PLAYER: Bonus de velocidade terminou.");
        }
    }

    player->position.x += input.x * currentSpeed * dt;
    player->position.y += input.y * currentSpeed * dt;

    // NOVO: Clampear a posição do jogador para os limites da tela
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Calcula o hitbox temporariamente para obter as dimensões corretas
    // (A hitbox final será calculada novamente após o clampeamento)
    Rectangle tempHitbox = GetPlayerHitbox(player);
    float halfWidth = tempHitbox.width / 2.0f;
    float halfHeight = tempHitbox.height / 2.0f;

    // Clampear a posição X
    player->position.x = fmaxf(halfWidth, fminf(player->position.x, screenWidth - halfWidth));
    // Clampear a posição Y
    player->position.y = fmaxf(halfHeight, fminf(player->position.y, screenHeight - halfHeight));


    int totalFramesForCurrentAnimation = GetFrameCountForState(player->currentState);

    player->frameTimer += dt;
    if (player->frameTimer >= player->frameSpeed) {
        player->frameTimer = 0.0f;
        player->currentFrame++;
        if (player->currentFrame >= totalFramesForCurrentAnimation) {
            player->currentFrame = 0;
        }
    }

    if (player->isInvulnerable) {
        player->invulnerableTimer += dt;
        if (player->invulnerableTimer >= player->invulnerableDuration) {
            player->isInvulnerable = false;
            player->invulnerableTimer = 0.0f;
            TraceLog(LOG_INFO, "PLAYER: Invulnerabilidade terminou.");
        }
    }

    if (player->bombCount < player->maxBombs) {
        player->bombRechargeTimer += dt;
        if (player->bombRechargeTimer >= player->bombRechargeInterval) {
            player->bombCount++;
            player->bombRechargeTimer = 0.0f;
            TraceLog(LOG_INFO, "PLAYER: Bomba recarregada! Total: %d", player->bombCount);
        }
    }

    // Recalcula o hitbox após a posição ter sido clampeada
    player->hitbox = GetPlayerHitbox(player);
}

void DrawPlayer(const Player* player, const GameAssets* assets) {
    Texture2D currentSheet;

    if (player->currentState == PLAYER_STATE_RUN) {
        currentSheet = assets->playerRunSheet;
    } else if (player->currentState == PLAYER_STATE_IDLE) {
        currentSheet = assets->playerIdleSheet;
    } else {
        currentSheet = assets->playerHitDeathSheet;
    }

    if (currentSheet.id == 0) {
        TraceLog(LOG_ERROR, "DRAW_PLAYER_ERROR: Textura do player inválida! Verifique logs de carregamento.");
        DrawRectangle(player->position.x - (TILE_SIZE * 3 / 2), player->position.y - (TILE_SIZE * 3 / 2), TILE_SIZE * 3, TILE_SIZE * 3, BLUE);
        return;
    }

    int sourceRow = 0;
    if (player->currentState != PLAYER_STATE_HIT_DEATH) {
        sourceRow = GetFrameRowForDirection(player->currentDirection);
    }

    float sourceRecYOffset = 0.0f;
    // Ajustes para as spritesheets (se aplicável, com base na sua imagem)
    // if (player->currentDirection == PLAYER_DIR_UP || player->currentDirection == PLAYER_DIR_LEFT) {
    //     sourceRecYOffset = 1.0f;
    // }

    Rectangle sourceRec = {
        (float)player->currentFrame * TILE_SIZE,
        (float)sourceRow * TILE_SIZE + sourceRecYOffset,
        (float)TILE_SIZE,
        (float)TILE_SIZE
    };

    float scale = 3.0f;

    Rectangle destRec = {
        player->position.x,
        player->position.y,
        (float)TILE_SIZE * scale,
        (float)TILE_SIZE * scale
    };

    Vector2 origin = { (float)TILE_SIZE / 2 * scale, (float)TILE_SIZE / 2 * scale };

    Color drawColor = WHITE;
    if (player->isInvulnerable) {
        if ((int)(player->invulnerableTimer * 10) % 2 == 0) {
            drawColor = (Color){255, 255, 255, 150};
        }
    }

    DrawTexturePro(currentSheet, sourceRec, destRec, origin, 0.0f, drawColor);

    #ifdef _DEBUG
        DrawRectangleLinesEx(player->hitbox, 1, GREEN);
    #endif
}

Rectangle GetPlayerHitbox(const Player* player) {
    // Estas são as dimensões da hitbox relativas ao centro do player sprite.
    // O sprite do player tem escala de 3.0, então TILE_SIZE * 3 = 48 pixels
    float scaledSpriteWidth = TILE_SIZE * 3;
    float scaledSpriteHeight = TILE_SIZE * 3;

    // Ajustes de hitbox para o sprite do player
    // Exemplo: 50% da largura do sprite, 70% da altura do sprite
    float hitboxWidth = scaledSpriteWidth * 0.5f;
    float hitboxHeight = scaledSpriteHeight * 0.7f;

    // Offset para centralizar a hitbox ou alinhá-la ao "pé" do player
    // Se o Y é o centro do sprite, e queremos que a hitbox esteja mais abaixo:
    float offsetY = scaledSpriteHeight * 0.15f; // Move a hitbox para baixo 15% da altura do sprite

    return (Rectangle){
        player->position.x - hitboxWidth / 2,         // X do canto superior esquerdo
        player->position.y - hitboxHeight / 2 + offsetY, // Y do canto superior esquerdo
        hitboxWidth,
        hitboxHeight
    };
}

void PlayerTakeDamage(Player* player, int damage) {
    if (player->isInvulnerable) return;

    player->health -= damage;
    TraceLog(LOG_INFO, "PLAYER: Dano recebido! Vida atual: %d", player->health);

    player->isInvulnerable = true;
    player->invulnerableTimer = 0.0f;

    player->escapeSpeedTimer = player->escapeSpeedDuration;
    TraceLog(LOG_INFO, "PLAYER: Bonus de velocidade ativado por %.2f segundos.", player->escapeSpeedDuration);

    if (player->health <= 0) {
        TraceLog(LOG_INFO, "PLAYER: Jogador morreu!");
    }
}

void PlayerApplyBuff(Player* player, BuffType type, float value) {
    switch (type) {
        case BUFF_TYPE_SPEED:
            player->speed *= (1.0f + (value / 100.0f));
            TraceLog(LOG_INFO, "PLAYER_BUFF: Velocidade aumentada para %.2f", player->speed);
            break;
        case BUFF_TYPE_HEALTH:
            player->maxHealth += (int)value;
            player->health += (int)value;
            if (player->health > player->maxHealth) player->health = player->maxHealth;
            TraceLog(LOG_INFO, "PLAYER_BUFF: Vida maxima aumentada para %d e vida atual curada para %d", player->maxHealth, player->health);
            break;
        case BUFF_TYPE_DAMAGE:
            player->baseDamageMultiplier *= (1.0f + (value / 100.0f));
            player->bombBaseDamage = (int)(player->bombBaseDamage * (1.0f + (value / 100.0f)));
            TraceLog(LOG_INFO, "PLAYER_BUFF: Dano aumentado. Multiplicador: %.2f, Dano Bomba: %d", player->baseDamageMultiplier, player->bombBaseDamage);
            break;
        case BUFF_TYPE_BOMB_CAP:
            player->maxBombs += (int)value;
            player->bombCount = player->maxBombs;
            player->bombsPerClick += (int)value;
            if (player->bombsPerClick > 2) player->bombsPerClick = 2; // Limite de 2 bombas por clique por enquanto
            TraceLog(LOG_INFO, "PLAYER_BUFF: Capacidade de bombas aumentada para %d. Bombs per click: %d", player->maxBombs, player->bombsPerClick);
            break;
        default:
            TraceLog(LOG_WARNING, "PLAYER_BUFF: Tipo de buff desconhecido: %d", type);
            break;
    }
}