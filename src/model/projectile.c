#include "../includes/projectile.h"
#include <stdlib.h>

Projectiles *createProjectiles(unsigned count) {
  Projectiles *projectiles = (Projectiles *)malloc(sizeof(Projectiles));
  if (!projectiles) {
    // handle error will do that later
    return NULL;
  }
  projectiles->count = MAX_PROJECTILES;
  for (unsigned i = 0; i < projectiles->count; i++) {
    projectiles->projectiles[i].active = false;
  }
  return projectiles;
}

void destroyProjectiles(Projectiles *projectiles) {
  if (!projectiles) {
    return;
  }
  free(projectiles);
}

void spawnProjectile(Projectiles *projectiles, float x, float y,
                     DirectionProjectile direction) {
  if (!projectiles) {
    return;
  }
  unsigned i;
  for (i = 0; i < projectiles->count; i++) {
    if (!projectiles->projectiles[i].active) {
      projectiles->projectiles[i].x = x;
      projectiles->projectiles[i].y = y;
      if (direction == MOVE_UP) {
        projectiles->projectiles[i].velocityY = -PROJECTILE_SPEED;
      } else if (direction == MOVE_DOWN) {
        projectiles->projectiles[i].velocityY = PROJECTILE_SPEED;
      } else {
        projectiles->projectiles[i].velocityY = 0.0f;
      }
      projectiles->projectiles[i].w = PROJECTILE_WIDTH;
      projectiles->projectiles[i].h = PROJECTILE_HEIGHT;
      projectiles->projectiles[i].active = true;
      break;
    }
  }
}

void updateProjectiles(Projectiles *projectiles, float deltaTime,
                       unsigned screenHeight) {
  if (!projectiles) {
    return;
  }
  for (unsigned i = 0; i < projectiles->count; i++) {
    if (projectiles->projectiles[i].active) {
      projectiles->projectiles[i].y +=
          projectiles->projectiles[i].velocityY * deltaTime;
      if (projectiles->projectiles[i].y < 0 ||
          projectiles->projectiles[i].y > screenHeight) {
        projectiles->projectiles[i].active = false;
      }
    }
  }
}