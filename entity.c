#include "level.h"
#include "entity.h"
#include "tileset.h"
#include <assert.h>
#include <math.h>

#define TILESIZE 16
#define GRAVITY 1.2f
#define WALKSPEED TILESIZE
#define BUNNYHOPSPEED TILESIZE * 2
#define JUMPHEIGHT TILESIZE * 4

bhop_Entity *player;

bhop_PlayerTiming playerTiming;

Rectangle _impl_bhop_Entity_getRect(bhop_Entity *entity) {
  return (Rectangle){.height = TILESIZE,
                     .width = TILESIZE,
                     .x = entity->origin.x,
                     .y = entity->origin.y};
}

char _impl_bhop_Entity_intersects(bhop_Entity *ent1, bhop_Entity *ent2) {
  return CheckCollisionRecs(_impl_bhop_Entity_getRect(ent1),
                            _impl_bhop_Entity_getRect(ent2));
}
void bhop_Entity_jump(bhop_Entity *entity)
{
    switch (entity->type) {
    case bhop_Entity_PLAYER:
        if (entity->collider & bhop_EntityCollider_$SOUTH) {
            entity->velocity = (Vector2){.x = BUNNYHOPSPEED, .y = JUMPHEIGHT};
        }
        break;
  case bhop_Entity_MUSTACHO: {
    if (entity->collider & bhop_EntityCollider_$SOUTH) {
      entity->velocity = (Vector2){.x = entity->velocity.x, .y = JUMPHEIGHT};
    }
    break;
  }
  default:
    assert(0);
  }
}

void bhop_Entity_walk(bhop_Entity *entity) {
  switch (entity->type) {
  case bhop_Entity_MUSTACHO: {
    if (entity->collider & bhop_EntityCollider_$SOUTH) {
      entity->velocity = (Vector2){.x = WALKSPEED, .y = entity->velocity.y};
    }
    break;
  }
  default:
    assert(0);
  }
}

void _impl_bhop_Entity_updateTileCollision(bhop_Entity *ent, bhop_Level *lvl)
{
    int clamp_x, clamp_y;
    int tile_x, tile_y;

    clamp_x = ((int) ent->origin.x) % TILESIZE;
    clamp_y = ((int) ent->origin.y) % TILESIZE;

    tile_x = ((int) ent->origin.x) / TILESIZE;

    tile_y = ((int) ent->origin.y + ent->velocity.y) / TILESIZE;

    int tile_nw = bhop_Level_getTerrainTile(lvl, (Vector2) {.x = tile_x,     .y = tile_y - 1});
    int tile_ne = bhop_Level_getTerrainTile(lvl, (Vector2) {.x = tile_x + 1, .y = tile_y - 1});
    int tile_sw = bhop_Level_getTerrainTile(lvl, (Vector2) {.x = tile_x,     .y = tile_y});
    int tile_se = bhop_Level_getTerrainTile(lvl, (Vector2) {.x = tile_x + 1, .y = tile_y});

    ent->collider = 0;

    if (tile_nw == 0 && tile_ne == 0) { // Corners
        if (tile_sw || tile_se) {
            ent->collider ^= bhop_EntityCollider_$SOUTH;
            ent->origin.y -= clamp_y;
        }
    } else {
        if (tile_sw && tile_se) {
            ent->collider ^= bhop_EntityCollider_$SOUTH;
            ent->origin.y -= clamp_y;
        }
    }

    if (tile_sw && tile_nw) {
        ent->collider ^= bhop_EntityCollider_$WEST;
        ent->origin.x += TILESIZE - clamp_x;
    }

    if (tile_se && tile_ne) {
        ent->collider ^= bhop_EntityCollider_$EAST;
        ent->origin.x -= clamp_x;
    }

    if (tile_nw && tile_ne)
        ent->collider ^= bhop_EntityCollider_$NORTH;
}

void _impl_bhop_Entity_updateMovement(bhop_Entity *ent)
{
    ent->origin.x += ent->velocity.x;
    ent->origin.y += ent->velocity.y;

    if (bhop_Entity_isBouncy$(*ent)) {
        if (ent->collider & bhop_EntityCollider_$SOUTH) {
            ent->velocity.y = - 8.0f;
        }
        if (ent->collider & bhop_EntityCollider_$WEST) {
            ent->velocity.x = fabs(ent->velocity.x);
        } else if (ent->collider & bhop_EntityCollider_$EAST) {
            ent->velocity.x = - fabs(ent->velocity.x);
        }
    } else {
        if (ent->collider & bhop_EntityCollider_$SOUTH) {
            ent->velocity.y = 0.0f;
        }
        if (ent->collider & (bhop_EntityCollider_$WEST | bhop_EntityCollider_$EAST))
            ent->velocity.x = 0.0f;
    }

    if (ent->collider & bhop_EntityCollider_$NORTH)
        ent->velocity.y = 0.0f;
    if (! (ent->collider & bhop_EntityCollider_$SOUTH)) {
        ent->velocity.y += GRAVITY;
    }
}

void bhop_updateEntities(bhop_Level *lvl)
{
    bhop_Entity *player = bhop_Level_getPlayerEntity(lvl);

    for (int i = 0; i < lvl->entities_count; i++) {
        bhop_Entity *ent = &lvl->entities[i];

        if (ent->type == bhop_Entity_NULL)
            continue;

        if (! bhop_Entity_isFixed$(*ent)) {
            _impl_bhop_Entity_updateTileCollision(ent, lvl);
            _impl_bhop_Entity_updateMovement(ent);
        }

        if (ent == player) {
            if (playerTiming.cdSouth)
                playerTiming.cdSouth -= 1;
            if (playerTiming.cdWest)
                playerTiming.cdWest -= 1;
            if (playerTiming.cdEast)
                playerTiming.cdEast -= 1;
            if (playerTiming.cdNorth)
                playerTiming.cdNorth -= 1;

            if (player->collider & bhop_EntityCollider_$SOUTH)
                playerTiming.cdSouth = 3;
            if (player->collider & bhop_EntityCollider_$NORTH)
                playerTiming.cdNorth = 3;
            if (player->collider & bhop_EntityCollider_$EAST)
                playerTiming.cdEast = 3;
            if (player->collider & bhop_EntityCollider_$WEST)
                playerTiming.cdWest = 3;

            continue;
        }

        if (_impl_bhop_Entity_intersects(player, ent)) {
            ent->type = bhop_Entity_NULL;
        }
    }

}

