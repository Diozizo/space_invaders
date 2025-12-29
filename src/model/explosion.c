#include "../../includes/explosion.h"
#include <stdlib.h>

ExplosionManager *createExplosionManager(void) {
  // Allocate zero-initialized memory
  ExplosionManager *em =
      (ExplosionManager *)calloc(1, sizeof(ExplosionManager));
  if (!em)
    return NULL;

  // Explicitly ensure all slots are inactive (redundant with calloc but good
  // for clarity)
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

  // Object Pool Logic: Find the first available (inactive) slot
  for (int i = 0; i < MAX_EXPLOSIONS; i++) {
    if (!em->explosions[i].active) {
      // Initialize the explosion at the given position
      em->explosions[i].x = x;
      em->explosions[i].y = y;

      // Reset timer to full duration
      em->explosions[i].timer = EXPLOSION_DURATION;

      // Start at the first animation frame
      em->explosions[i].currentFrame = 0;

      // Activate the slot
      em->explosions[i].active = true;
      return; // Exit as soon as we spawn one
    }
  }
  // If loop finishes without returning, the pool was full (explosion ignored).
}

void updateExplosions(ExplosionManager *em, float deltaTime) {
  if (!em)
    return;

  for (int i = 0; i < MAX_EXPLOSIONS; i++) {
    if (em->explosions[i].active) {
      // Decrease timer
      em->explosions[i].timer -= deltaTime;

      // Check if explosion has finished playing
      if (em->explosions[i].timer <= 0) {
        em->explosions[i].active = false;
      } else {
        // --- Animation Frame Calculation ---
        // We calculate which frame (0, 1, or 2) to show based on remaining
        // time. lifePercent goes from 1.0 (Start) -> 0.0 (End)
        float lifePercent = em->explosions[i].timer / EXPLOSION_DURATION;

        // Map the percentage to 3 discrete frames
        if (lifePercent > 0.66f)
          em->explosions[i].currentFrame = 0; // Start (Big/Bright)
        else if (lifePercent > 0.33f)
          em->explosions[i].currentFrame = 1; // Middle
        else
          em->explosions[i].currentFrame = 2; // End (Fading/Small)
      }
    }
  }
}