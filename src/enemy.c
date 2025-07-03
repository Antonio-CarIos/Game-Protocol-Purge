#include "enemy.h"
#include "asset_manager.h"
#include "player.h" 
#include "raymath.h" 
#include "raylib.h"
#include <math.h> 

// --- Funções Auxiliares (Estáticas) ---


static int GetEnemyFrameCountForState(EnemyAnimationState state) {
    switch (state) {
        case ENEMY_STATE_RUN:
            return 7;
        case ENEMY_STATE_DEATH:
            return 4; 
        default:
            return 1;
    }
}


static int GetEnemyFrameRowForDirection(EnemyDirection dir) {
    return (int)dir; 
}

// --- Funções Principais do Inimigo ---

void InitEnemy(Enemy* enemy) {
    enemy->position = (Vector2){0, 0}; // Posição inicial (será definida ao spawnar)
    enemy->targetPosition = (Vector2){0, 0};
    enemy->speed = 80.0f;              // Velocidade padrão dos inimigos
    enemy->active = false;             // Começa inativo

    enemy->currentState = ENEMY_STATE_RUN; // Inimigos geralmente só "correm"
    enemy->currentDirection = ENEMY_DIR_DOWN; // Direção padrão

    enemy->currentFrame = 0;
    enemy->frameTimer = 0.0f;
    enemy->frameSpeed = 0.15f; // Velocidade da animação do inimigo

    // A hitbox inicializada aqui, mas atualizada em GetEnemyHitbox
    enemy->hitbox = (Rectangle){0, 0, 0, 0};

    // --- Inicialização de atributos de vida do inimigo ---
    enemy->health = 50; // Vida inicial do inimigo (pode ser ajustada por tipo de inimigo)
    enemy->isInvulnerable = false;
    enemy->invulnerableTimer = 0.0f;
    enemy->invulnerableDuration = 0.5f; // Curta invulnerabilidade para inimigos
    
    enemy->xpValue = 5; // NOVO: Cada inimigo concede 5 de XP
}

void UpdateEnemy(Enemy* enemy, float dt, const Player* player) {
    if (!enemy->active) return; 

    enemy->targetPosition = player->position; // O alvo do inimigo é a posição do player

    // Calcula o vetor de direção do inimigo para o player
    Vector2 direction = Vector2Subtract(enemy->targetPosition, enemy->position);

    // Atualiza a direção do inimigo para a animação
    if (fabs(direction.x) > fabs(direction.y)) { 
        if (direction.x > 0) enemy->currentDirection = ENEMY_DIR_RIGHT;
        else enemy->currentDirection = ENEMY_DIR_LEFT;
    } else { 
        if (direction.y > 0) enemy->currentDirection = ENEMY_DIR_DOWN;
        else enemy->currentDirection = ENEMY_DIR_UP;
    }

    
    direction = Vector2Normalize(direction);

    // Move o inimigo na direção do player
    enemy->position.x += direction.x * enemy->speed * dt;
    enemy->position.y += direction.y * enemy->speed * dt;

    // Lógica de atualização de frame da animação
    int totalFramesForCurrentAnimation = GetEnemyFrameCountForState(enemy->currentState);

    enemy->frameTimer += dt;
    if (enemy->frameTimer >= enemy->frameSpeed) {
        enemy->frameTimer = 0.0f;
        enemy->currentFrame++;
        if (enemy->currentFrame >= totalFramesForCurrentAnimation) {
            enemy->currentFrame = 0; 
        }
    }

    // --- Lógica de invulnerabilidade do inimigo ---
    if (enemy->isInvulnerable) {
        enemy->invulnerableTimer += dt;
        if (enemy->invulnerableTimer >= enemy->invulnerableDuration) {
            enemy->isInvulnerable = false;
            enemy->invulnerableTimer = 0.0f;
            
        }
    }

    // Atualiza a hitbox do inimigo a cada frame
    enemy->hitbox = GetEnemyHitbox(enemy);
}

void DrawEnemy(const Enemy* enemy, const GameAssets* assets) {
    if (!enemy->active) return; 

    Texture2D currentSheet = assets->snakeFullSheet; // Usa a textura carregada em asset_manager.c

    // DEBUG VISUAL: Se a textura da cobra não carregou, desenha um quadrado vermelho
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

    // Fator de escala (3x para combinar com o player)
    float scale = 3.0f;

    // Retângulo de destino na tela
    Rectangle destRec = {
        enemy->position.x,
        enemy->position.y,
        (float)TILE_SIZE * scale,
        (float)TILE_SIZE * scale
    };

    // Ponto de origem para centralizar o sprite
    Vector2 origin = { (float)TILE_SIZE / 2 * scale, (float)TILE_SIZE / 2 * scale };

    Color drawColor = WHITE;
    if (enemy->isInvulnerable) {
        // Faz o inimigo piscar durante a invulnerabilidade
        if ((int)(enemy->invulnerableTimer * 10) % 2 == 0) {
            drawColor = (Color){255, 255, 255, 150}; // Transparente
        }
    }

    DrawTexturePro(currentSheet, sourceRec, destRec, origin, 0.0f, drawColor);

    // DEBUG: Desenha o hitbox do inimigo
    #ifdef _DEBUG
        DrawRectangleLinesEx(enemy->hitbox, 1, RED);
        // Opcional: desenhar a vida do inimigo acima dele (para debug)
        DrawText(TextFormat("%d", enemy->health), (int)enemy->position.x - 10, (int)enemy->position.y - 40, 10, WHITE);
    #endif
}

// --- Funções Auxiliares de Colisão ---

Rectangle GetEnemyHitbox(const Enemy* enemy) {
    // Ajusta a hitbox para ser menor que o sprite da cobra.
    float hitboxWidth = TILE_SIZE * 3 * 0.6f; 
    float hitboxHeight = TILE_SIZE * 3 * 0.5f; 

 
    return (Rectangle){
        enemy->position.x - hitboxWidth / 2,
        enemy->position.y - hitboxHeight / 2,
        hitboxWidth,
        hitboxHeight
    };
}

// MODIFICADO: Retorna o XP do inimigo se ele for derrotado, 0 caso contrário
int EnemyTakeDamage(Enemy* enemy, int damage) {
    if (enemy->isInvulnerable || !enemy->active) return 0; // Não leva dano se estiver invulnerável ou inativo

    enemy->health -= damage;
    TraceLog(LOG_INFO, "ENEMY: Inimigo recebeu %d de dano! Vida atual: %d", damage, enemy->health);

    enemy->isInvulnerable = true;
    enemy->invulnerableTimer = 0.0f; 

    if (enemy->health <= 0) {
        enemy->active = false; 
        TraceLog(LOG_INFO, "ENEMY: Inimigo derrotado!");
        return enemy->xpValue; // Retorna o XP do inimigo
    }
    return 0; // Não derrotado, não concede XP
}