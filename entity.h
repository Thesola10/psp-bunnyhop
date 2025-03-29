#include <raylib.h>


// texture <- (composite)

typedef enum {
  bhop_Entity_NULL = 0,
  bhop_Entity_PLAYER,
  bhop_Entity_MUSTACHO,
  bhop_Entity_EGG,
} bhop_EntityType;

typedef struct {
  bhop_EntityType type;
  Vector2 origin;
  Vector2 direction;
  char is_fixed;
} bhop_Entity;
float gravity;

//// FUNCTION

// JUMP force y, x depend Mustacho/Bunny

// WALK creeper

// update

// draw

// check_collision