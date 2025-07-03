// src/map.h
#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include "types.h"         // Contém a definição da struct GameMap e enums
#include "asset_manager.h" // Para GameAssets

// Declarações das funções do mapa
void InitMap(GameMap* map);
void DrawMap(const GameMap* map, const GameAssets* assets);

#endif // MAP_H