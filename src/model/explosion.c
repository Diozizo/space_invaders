#include "../includes/explosion.h"
#include <stdlib.h>

ExplosionManager *createExplosionManager(void) {
  ExplosionManager *em = (ExplosionManager *)malloc(sizeof(ExplosionManager));
  if (!em)
    return NULL;

  for (int i = 0; i < MAX_EXPLOSIONS; i++) {
    em->explosions[i].active = false;
  }
  return em;
}

void destroyExplosionManager(ExplosionManager *em) {
  if (em)
    free(em);
}

void spawnExplosion(ExplosionManager *em, float x, float y) {
  if (!em)
    return;

  // Find first inactive slot
  for (int i = 0; i < MAX_EXPLOSIONS; i++) {
    if (!em->explosions[i].active) {
      em->explosions[i].x = x;
      em->explosions[i].y = y;
      em->explosions[i].timer = EXPLOSION_DURATION;
      em->explosions[i].currentFrame = 0;
      em->explosions[i].active = true;
      return;
    }
  }
}

void updateExplosions(ExplosionManager *em, float deltaTime) {
  if (!em)
    return;

  for (int i = 0; i < MAX_EXPLOSIONS; i++) {
    if (em->explosions[i].active) {
      em->explosions[i].timer -= deltaTime;

      if (em->explosions[i].timer <= 0) {
        em->explosions[i].active = false;
      } else {
        // Calculate animation frame (0, 1, or 2)
        // We divide the lifespan into 3 chunks
        float lifePercent = em->explosions[i].timer / EXPLOSION_DURATION;

        if (lifePercent > 0.66f)
          em->explosions[i].currentFrame = 0;
        else if (lifePercent > 0.33f)
          em->explosions[i].currentFrame = 1;
        else
          em->explosions[i].currentFrame = 2;
      }
    }
  }
}