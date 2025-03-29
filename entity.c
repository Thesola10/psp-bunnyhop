#include "tileset.h"
#include "entity.h"
#include <assert.h>

#define TILESIZE 16
#define GRAVITY -TILESIZE * 0.1
#define WALKSPEED TILESIZE
#define BUNNYHOPSPEED TILESIZE * 2
#define JUMPHEIGHT TILESIZE * 4

bhop_Entity *player;

Rectangle _impl_bhop_Entity_getRect(bhop_Entity *entity) {
  return (Rectangle){.height = TILESIZE,
                     .width = TILESIZE,
                     .x = entity->origin.x,
                     .y = entity->origin.y};
}
char _impl_bhop_Entity_isCollided(bhop_Entity *entity) {
  return CheckCollisionRecs(_impl_bhop_Entity_getRect(player),
                            _impl_bhop_Entity_getRect(entity));
}
void bhop_Entity_jump(bhop_Entity *entity) {
  switch (entity->type) {
  case bhop_Entity_PLAYER: {
    if (entity->is_on_ground) {
      entity->velocity = (Vector2){.x = BUNNYHOPSPEED, .y = JUMPHEIGHT};
    }
    break;
  }
  case bhop_Entity_MUSTACHO: {
    if (entity->is_on_ground) {
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
    if (entity->is_on_ground) {
      entity->velocity = (Vector2){.x = WALKSPEED, .y = entity->velocity.y};
    }
    break;
  }
  default:
    assert(0);
  }
}

void bhop_updateEntities(bhop_Level *lvl)
{

}

