#include <stdlib.h>

#include "level.h"

#include "tileset.h"

Texture2D bhop_levelTileset;
Image bhop_levelTilesetImg;

bhop_Entity *playerEntity = 0;

bhop_Level *currentLevel;

void bhop_loadLevelTileset(Image tileset)
{
    bhop_levelTileset = LoadTextureFromImage(tileset);
    bhop_levelTilesetImg = tileset;
}

void bhop_Level_drawEntities(bhop_Level *lvl)
{
    for (int i = 0; i < lvl->entities_count; i++) {
        bhop_Entity ent = lvl->entities[i];
        Vector2 realPos = (Vector2) { .x = ent.origin.x, .y = ent.origin.y - TILESIZE };
        bhop_drawTile(bhop_levelTileset, ent.type, realPos);
    }
}

void bhop_Level_load(bhop_Level *lvl)
{
    lvl->entities = malloc(lvl->entities_count * sizeof(bhop_Entity));

    for (int i = 0; i < lvl->entities_count; i++) {
        lvl->entities[i] = lvl->entities_orig[i];
    }

    for (int i = 0; i < lvl->entities_count; i++) {
        if (lvl->entities[i].type == bhop_Entity_MUSTACHO)
            lvl->entities[i].velocity.x = 3.0f;
    }

    currentLevel = lvl;
}

bhop_Level *bhop_getCurrentLevel(void)
{
    return currentLevel;
}

Texture2D bhop_Level_getTerrainTexture(bhop_Level *lvl)
{
    Image target;

    if (lvl->terrainLayer.lifetime != 0)
        return lvl->terrainLayer.output;

    if (IsTextureValid(lvl->terrainLayer.output))
        UnloadTexture(lvl->terrainLayer.output);

    target = GenImageColor(512, 512, ColorAlpha(WHITE, 0.0f));

    for (int j = 0; j < lvl->terrain_size.y; j++) {
        for (int i = 0; i < lvl->terrain_size.x; i++) {
            int ncell = (j * lvl->terrain_size.x) + i;
            bhop_drawTileToImage(&bhop_levelTilesetImg, lvl->terrain[ncell],
                    (Vector2) { i * TILESIZE, j * TILESIZE }, &target);
        }
    }

    lvl->terrainLayer.output = LoadTextureFromImage(target);
    lvl->terrainLayer.lifetime = 120;

    UnloadImage(target);
    return lvl->terrainLayer.output;
}

Texture2D bhop_Level_getDecorTexture(bhop_Level *lvl)
{
    Image target;

    if (lvl->decorLayer.lifetime != 0)
        return lvl->decorLayer.output;

    if (IsTextureValid(lvl->decorLayer.output))
        UnloadTexture(lvl->decorLayer.output);

    target = GenImageColor(512, 512, ColorAlpha(WHITE, 0.0f));

    for (int j = 0; j < lvl->decor_size.y; j++) {
        for (int i = 0; i < lvl->decor_size.x; i++) {
            int ncell = (j * lvl->decor_size.x) + i;
            bhop_drawTileToImage(&bhop_levelTilesetImg, lvl->decor[ncell],
                    (Vector2) { i * TILESIZE, j * TILESIZE }, &target);
        }
    }

    lvl->decorLayer.output = LoadTextureFromImage(target);
    lvl->decorLayer.lifetime = 120;

    UnloadImage(target);
    return lvl->decorLayer.output;
}


int bhop_Level_getTerrainTile(bhop_Level *lvl, Vector2 where)
{
    int tile_idx = (where.y * lvl->terrain_size.x) + where.x;

    return lvl->terrain[tile_idx];
}

bhop_Entity *bhop_Level_getPlayerEntity(bhop_Level *lvl)
{
    if (playerEntity)
        return playerEntity;

    for (int i = 0; i < lvl->entities_count; i++) {
        if (lvl->entities[i].type == bhop_Entity_PLAYER) {
            playerEntity = &lvl->entities[i];
            break;
        }
    }

    return playerEntity;
}
