#include "player.h"
#include "asset_manager.h" 
#include "raylib.h"
#include "raymath.h" 
#include <math.h> 

// --- Funções Auxiliares (Estáticas) ---

static int GetFrameCountForState(PlayerAnimationState state) {
    switch (state) {
        case PLAYER_STATE_IDLE:
            return 8;
        case PLAYER_STATE_RUN:
            return 8;
        case PLAYER_STATE_HIT_DEATH:
            return 4; // Assumindo 4 frames para hit/death
        default:
            return 1;
    }
}

static int GetFrameRowForDirection(PlayerDirection dir) {
    return (int)dir; 
}

// --- Funções Principais do Player ---

void InitPlayer(Player* player, Vector2 startPos) {
    player->position = startPos;
    player->speed = 100.0f; // Velocidade BASE de movimento do player
    player->currentState = PLAYER_STATE_IDLE;
    player->currentDirection = PLAYER_DIR_DOWN;

    player->currentFrame = 0;
    player->frameTimer = 0.0f;
    player->frameSpeed = 0.1f; // Velocidade da animação (100ms por frame)

    // --- Inicialização de atributos de colisão e vida ---
    player->health = 100; // Vida inicial do jogador
    player->isInvulnerable = false;
    player->invulnerableTimer = 0.0f;
    player->invulnerableDuration = 1.5f; // 1.5 segundos de invulnerabilidade após levar dano 

    // A hitbox inicializada aqui, mas atualizada em GetPlayerHitbox
    player->hitbox = (Rectangle){0, 0, 0, 0};

    // --- Inicialização de atributos para velocidade de escape ---
    player->escapeSpeedBonus = 70.0f; // 70 unidades de velocidade extra (aumentado um pouco)
    player->escapeSpeedTimer = 0.0f;
    player->escapeSpeedDuration = 1.0f; // 1.0 segundo de velocidade bônus 

    // --- INICIALIZAÇÃO DE ATRIBUTOS PARA BOMBAS ---
    player->maxBombs = 5;               // Máximo de 5 bombas
    player->bombCount = player->maxBombs; // Começa com o máximo de bombas
    player->bombRechargeTimer = 0.0f;
    player->bombRechargeInterval = 5.0f; // Recarrega uma bomba a cada 5 segundos
}

void UpdatePlayer(Player* player, float dt) {
    Vector2 input = {0.0f, 0.0f};

    if (IsKeyDown(KEY_W)) input.y -= 1.0f; // Cima
    if (IsKeyDown(KEY_S)) input.y += 1.0f; // Baixo
    if (IsKeyDown(KEY_A)) input.x -= 1.0f; // Esquerda
    if (IsKeyDown(KEY_D)) input.x += 1.0f; // Direita

    // Normaliza o vetor de input para movimento diagonal 
    if (Vector2Length(input) > 0) {
        input = Vector2Normalize(input);
        player->currentState = PLAYER_STATE_RUN; // Mudar para estado de corrida
    } else {
        player->currentState = PLAYER_STATE_IDLE; // Mudar para estado parado
    }

    // Atualiza a direção do player para a animação
    if (fabs(input.x) > fabs(input.y)) { 
        if (input.x > 0) player->currentDirection = PLAYER_DIR_RIGHT;
        else if (input.x < 0) player->currentDirection = PLAYER_DIR_LEFT;
    } else if (fabs(input.y) > 0) { 
        if (input.y > 0) player->currentDirection = PLAYER_DIR_DOWN;
        else if (input.y < 0) player->currentDirection = PLAYER_DIR_UP;
    }
   

    // --- Lógica da velocidade de escape ---
    float currentSpeed = player->speed;
    if (player->escapeSpeedTimer > 0) {
        currentSpeed += player->escapeSpeedBonus;
        player->escapeSpeedTimer -= dt;
        if (player->escapeSpeedTimer <= 0) {
            player->escapeSpeedTimer = 0.0f; // Garante que não fica negativo
            TraceLog(LOG_INFO, "PLAYER: Bonus de velocidade terminou.");
        }
    }

    // Atualiza a posição do player com a velocidade atualizada
    player->position.x += input.x * currentSpeed * dt;
    player->position.y += input.y * currentSpeed * dt;

    // Lógica de atualização de frame da animação
    int totalFramesForCurrentAnimation = GetFrameCountForState(player->currentState);

    player->frameTimer += dt;
    if (player->frameTimer >= player->frameSpeed) {
        player->frameTimer = 0.0f;
        player->currentFrame++;
        if (player->currentFrame >= totalFramesForCurrentAnimation) {
            player->currentFrame = 0; 
        }
    }

    // --- Lógica de invulnerabilidade ---
    if (player->isInvulnerable) {
        player->invulnerableTimer += dt;
        if (player->invulnerableTimer >= player->invulnerableDuration) {
            player->isInvulnerable = false;
            player->invulnerableTimer = 0.0f;
            TraceLog(LOG_INFO, "PLAYER: Invulnerabilidade terminou.");
        }
    }

    // --- Lógica de recarga de bombas ---
    if (player->bombCount < player->maxBombs) {
        player->bombRechargeTimer += dt;
        if (player->bombRechargeTimer >= player->bombRechargeInterval) {
            player->bombCount++;
            player->bombRechargeTimer = 0.0f; // Reseta para a próxima recarga
            TraceLog(LOG_INFO, "PLAYER: Bomba recarregada! Total: %d", player->bombCount);
        }
    }

    // Atualiza a hitbox do player a cada frame 
    player->hitbox = GetPlayerHitbox(player);
}

void DrawPlayer(const Player* player, const GameAssets* assets) {
    Texture2D currentSheet;

    switch (player->currentState) {
        case PLAYER_STATE_IDLE:
            currentSheet = assets->playerIdleSheet;
            break;
        case PLAYER_STATE_RUN:
            currentSheet = assets->playerRunSheet;
            break;
        case PLAYER_STATE_HIT_DEATH:
            currentSheet = assets->playerHitDeathSheet;
            break;
        default:
            currentSheet = assets->playerIdleSheet;
            break;
    }

    // DEBUG VISUAL: Se a textura do player não carregou, desenha um quadrado azul
    if (currentSheet.id == 0) {
        TraceLog(LOG_ERROR, "DRAW_PLAYER_ERROR: Textura do player inválida! Verifique logs de carregamento.");
        DrawRectangle(player->position.x - (TILE_SIZE * 3 / 2), player->position.y - (TILE_SIZE * 3 / 2), TILE_SIZE * 3, TILE_SIZE * 3, BLUE);
        return;
    }

    int sourceRow = GetFrameRowForDirection(player->currentDirection);
   
    if (player->currentState == PLAYER_STATE_HIT_DEATH) {
        sourceRow = 0;
    }

    // Retângulo de origem na spritesheet
    Rectangle sourceRec = {
        (float)player->currentFrame * TILE_SIZE,
        (float)sourceRow * TILE_SIZE,
        (float)TILE_SIZE,
        (float)TILE_SIZE
    };

    // Fator de escala (3x para tornar o personagem maior)
    float scale = 3.0f;

  
    Rectangle destRec = {
        player->position.x,
        player->position.y,
        (float)TILE_SIZE * scale,
        (float)TILE_SIZE * scale
    };

    // Ponto de origem para centralizar o sprite (centro do sprite)
    Vector2 origin = { (float)TILE_SIZE / 2 * scale, (float)TILE_SIZE / 2 * scale };

    Color drawColor = WHITE;
    if (player->isInvulnerable) {
        // Faz o player piscar durante a invulnerabilidade
        if ((int)(player->invulnerableTimer * 10) % 2 == 0) {
            drawColor = (Color){255, 255, 255, 150}; // Transparente
        }
    }

    DrawTexturePro(currentSheet, sourceRec, destRec, origin, 0.0f, drawColor);

    // DEBUG: Desenha o hitbox do player
    #ifdef _DEBUG
        DrawRectangleLinesEx(player->hitbox, 1, GREEN);
    #endif
}

// --- Funções Auxiliares de Colisão e Dano ---

Rectangle GetPlayerHitbox(const Player* player) {
    // A hitbox pode ser um pouco menor que o sprite visível para uma sensação de jogo melhor.
    // TILE_SIZE * scale é o tamanho total do sprite desenhado.
    float hitboxWidth = TILE_SIZE * 3 * 0.5f; // 50% da largura escalada
    float hitboxHeight = TILE_SIZE * 3 * 0.7f; // 70% da altura escalada (parte de baixo do personagem)

    // Centraliza a hitbox na parte inferior do sprite
    return (Rectangle){
        player->position.x - hitboxWidth / 2,
        player->position.y - hitboxHeight / 2 + (TILE_SIZE * 3 * 0.1f), 
        hitboxWidth,
        hitboxHeight
    };
}

// Removido knockbackDirection e knockbackForce
void PlayerTakeDamage(Player* player, int damage) {
    if (player->isInvulnerable) return; // Não leva dano se estiver invulnerável

    player->health -= damage;
    TraceLog(LOG_INFO, "PLAYER: Dano recebido! Vida atual: %d", player->health);

    player->isInvulnerable = true;
    player->invulnerableTimer = 0.0f; // Reinicia o timer de invulnerabilidade

    // Ativa o bônus de velocidade de escape
    player->escapeSpeedTimer = player->escapeSpeedDuration;
    TraceLog(LOG_INFO, "PLAYER: Bonus de velocidade ativado por %.2f segundos.", player->escapeSpeedDuration);

    if (player->health <= 0) {
        TraceLog(LOG_INFO, "PLAYER: Jogador morreu!");
        // Lógica de game over aqui (ou mudar estado do player para "morto")
    }
}