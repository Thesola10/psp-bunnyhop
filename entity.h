#ifndef __ENTITY_H
#define __ENTITY_H

#include <raylib.h>

typedef struct _impl_bhop_Level bhop_Level;

typedef enum {
  bhop_Entity_NULL = 0,
  bhop_Entity_PLAYER = 16,
  bhop_Entity_MUSTACHO = 14,
  bhop_Entity_EGG = 13,
} bhop_EntityType;

#define bhop_Entity_isFixed$(x) \
    ((x).type == bhop_Entity_NULL || (x).type == bhop_Entity_EGG)

#define bhop_Entity_isBouncy$(x) \
    ((x).type == bhop_Entity_PLAYER)

/**
 * 1<<0: NORTH
 * 1<<1: SOUTH
 * 1<<2: EAST
 * 1<<3: WEST
 */
typedef char bhop_EntityCollider;

#define bhop_EntityCollider_$NORTH 1<<0
#define bhop_EntityCollider_$SOUTH 1<<1
#define bhop_EntityCollider_$EAST  1<<2
#define bhop_EntityCollider_$WEST  1<<3

typedef struct {
  bhop_EntityType type;
  Vector2 origin;
  Vector2 velocity;
  bhop_EntityCollider collider;
} bhop_Entity;

void bhop_Entity_jump(bhop_Entity *entity);
void bhop_Entity_walk(bhop_Entity *entity);
void bhop_updateEntities(bhop_Level *lvl);

#endif //__ENTITY_H
