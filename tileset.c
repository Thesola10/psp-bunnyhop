#include "tileset.h"

void bhop_drawTile(Texture2D tileset, int tileId, Vector2 where)
{
    int workId = tileId - 1;

    int tilex = workId % (tileset.width / TILESIZE);
    int tiley = workId / (tileset.width / TILESIZE);

    Rectangle subTile = (Rectangle) {
        .width = TILESIZE,
        .height = TILESIZE,
        .x = tilex * TILESIZE,
        .y = tiley * TILESIZE
    };

    if (!tileId) return;

    DrawTextureRec(tileset, subTile, where, WHITE);
}

void bhop_drawTileToImage(Image *tileset, int tileId, Vector2 where, Image *target)
{
    int workId = tileId - 1;

    int tilex = workId % (tileset->width / TILESIZE);
    int tiley = workId / (tileset->width / TILESIZE);

    Rectangle subTile = (Rectangle) {
        .width = TILESIZE,
        .height = TILESIZE,
        .x = tilex * TILESIZE,
        .y = tiley * TILESIZE
    };

    if (!tileId) return;

    ImageDraw(target, *tileset, subTile,
            (Rectangle) { .width = TILESIZE, .height = TILESIZE,
                          .x = where.x, .y = where.y
                        },
             WHITE);
}
