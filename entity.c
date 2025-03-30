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

int playerJumpCd = 0;

bhop_EntityEvent onPlayerJump = 0;
bhop_EntityEvent onPlayerWallJump = 0;
bhop_EntityEvent onPlayerBounce = 0;

bhop_EntityEvent onPlayerCollectCoins = 0;
bhop_EntityEvent onPlayerHitEnemy = 0;


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
            if (ent->type == bhop_Entity_PLAYER)
                onPlayerBounce(ent);
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

void bhop_Player_loadOnBounce(bhop_EntityEvent evt) {
    onPlayerBounce = evt;
}

void bhop_Player_loadOnJump(bhop_EntityEvent evt) {
    onPlayerJump = evt;
}

void bhop_Player_loadOnWallJump(bhop_EntityEvent evt) {
    onPlayerWallJump = evt;
}

void bhop_Player_loadOnHitEnemy(bhop_EntityEvent evt) {
    onPlayerHitEnemy = evt;
}

void bhop_Player_loadOnCollectCoins(bhop_EntityEvent evt) {
    onPlayerCollectCoins = evt;
}

void bhop_updateEntities(bhop_Level *lvl)
{
    bhop_Entity *player = bhop_Level_getPlayerEntity(lvl);

    if (playerJumpCd)
        playerJumpCd--;

    for (int i = 0; i < lvl->entities_count; i++) {
        bhop_Entity *ent = &lvl->entities[i];

        if (ent->type == bhop_Entity_NULL)
            continue;

        if (! bhop_Entity_isFixed$(*ent)) {
            _impl_bhop_Entity_updateTileCollision(ent, lvl);
            _impl_bhop_Entity_updateMovement(ent);
        }

        if (ent == player) {
            if (player->collider & bhop_EntityCollider_$SOUTH)
                if (playerJumpCd)
                    onPlayerJump(ent);

            if (player->collider & (bhop_EntityCollider_$WEST | bhop_EntityCollider_$EAST))
                if (playerJumpCd)
                    onPlayerWallJump(ent);


            continue;
        }

#define _impl_bhop_EntityEvent_callIfExists$(handler) { \
        if ( handler != (void*) 0 ) { handler ( ent ); } \
        else                        { TraceLog(LOG_INFO, "Empty handler: " #handler "\n" ); }\
    break; }

        if (_impl_bhop_Entity_intersects(player, ent)) {
            switch (ent->type) {
            case bhop_Entity_EGG:
                _impl_bhop_EntityEvent_callIfExists$(onPlayerCollectCoins);
            case bhop_Entity_MUSTACHO:
                _impl_bhop_EntityEvent_callIfExists$(onPlayerHitEnemy);
            default:
                TraceLog(LOG_FATAL, "Collided with PLAYER or NOTHING!!!");
            }
        }
    }

}

