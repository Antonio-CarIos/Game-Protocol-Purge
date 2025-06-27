#ifndef BOMB_H
#define BOMB_H

#include "raylib.h"
#include "asset_manager.h" 

typedef struct {
    Vector2 position;
    bool active;       
    
    // Timer para explosão
    float activationTimer;      // Contador para o tempo até explodir
    float activationDuration;  

    int currentFrame;           // Frame atual da animação
    float frameTimer;           // Timer para a troca de frames
    float frameSpeed;           // Velocidade da animação (tempo por frame)
    int frameCount;             // Número total de frames na animação
    bool hasExploded;           

    // --- ATRIBUTOS PARA DANO ---
    float damageRadius;         // Raio de alcance da explosão para dano
    int damageAmount;           // Quantidade de dano que a bomba causa
    bool damageApplied;         // Flag para garantir que o dano seja aplicado apenas uma vez por explosão

} Bomb;

// Declarações de funções da bomba
void InitBomb(Bomb* bomb);
void UpdateBomb(Bomb* bomb, float dt);
void DrawBomb(const Bomb* bomb, const GameAssets* assets);

#endif // BOMB_H