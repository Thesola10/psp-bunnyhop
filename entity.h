#include <raylib.h>

#include "levels/level.h"

typedef enum {
  bhop_Entity_NULL = 0,
  bhop_Entity_PLAYER,
  bhop_Entity_MUSTACHO,
  bhop_Entity_EGG,
} bhop_EntityType;

typedef struct {
  bhop_EntityType type;
  Vector2 origin;
  Vector2 velocity;
  char is_fixed;
  char is_on_ground;
} bhop_Entity;

void bhop_Entity_jump(bhop_Entity *entity);
void bhop_Entity_walk(bhop_Entity *entity);
void bhop_updateEntities(bhop_Level *lvl);
