#include "levels/level.h"

#include "tileset.h"

Texture2D bhop_levelTileset;
Image bhop_levelTilesetImg;

void bhop_loadLevelTileset(Texture2D tileset)
{
    bhop_levelTileset = tileset;
    bhop_levelTilesetImg = LoadImageFromTexture(tileset);
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
