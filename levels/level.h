#include "../entity.h"

typedef struct {
    Texture2D output;
    int lifetime;
} bhop_BakedLayer;

typedef struct {
    Vector2 terrain_size;
    Vector2 decor_size;
    int entities_count;
    char *terrain;
    char *decor;
    bhop_Entity *entities;

    bhop_BakedLayer terrainLayer;
    bhop_BakedLayer decorLayer;
} bhop_Level;


void bhop_loadLevelTileset(Texture2D tileset);

Texture2D bhop_Level_getTerrainTexture(bhop_Level *lvl);
Texture2D bhop_Level_getDecorTexture(bhop_Level *lvl);
