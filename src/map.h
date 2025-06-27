#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include "asset_manager.h" 


typedef struct {
 
} GameMap;

// Declarações das funções do mapa
void InitMap(GameMap* map);
void DrawMap(const GameMap* map, const GameAssets* assets);

#endif // MAP_H