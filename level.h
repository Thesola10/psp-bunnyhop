#ifndef __LEVEL_H
#define __LEVEL_H

#include "entity.h"

typedef struct {
    Texture2D output;
    int lifetime;
} bhop_BakedLayer;

struct _impl_bhop_Level {
    Vector2 terrain_size;
    Vector2 decor_size;
    int entities_count;
    char *terrain;
    char *decor;
    bhop_Entity *entities;

    bhop_Entity *entities_orig;
    bhop_BakedLayer terrainLayer;
    bhop_BakedLayer decorLayer;
};

typedef struct _impl_bhop_Level bhop_Level;


void bhop_loadLevelTileset(Image tileset);

void bhop_Level_load(bhop_Level*);

Texture2D bhop_Level_getTerrainTexture(bhop_Level *lvl);
Texture2D bhop_Level_getDecorTexture(bhop_Level *lvl);

void bhop_Level_drawEntities(bhop_Level *lvl);

bhop_Entity *bhop_Level_getPlayerEntity(bhop_Level *lvl);

int bhop_Level_getTerrainTile(bhop_Level *lvl, Vector2 where);


bhop_Level *bhop_getCurrentLevel(void);

#endif //__LEVEL_H
