#include "../../includes/projectile.h"
#include <stdlib.h>

Projectiles *createProjectiles(unsigned count) {
  // Allocate memory for the Manager + Array of Projectiles
  // Using calloc ensures everything starts at 0 (NULL/false)
  Projectiles *projectiles = (Projectiles *)calloc(1, sizeof(Projectiles));
  if (!projectiles) {
    return NULL;
  }

  // Initialize the Object Pool
  projectiles->count = MAX_PROJECTILES;
  for (unsigned i = 0; i < projectiles->count; i++) {
    projectiles->projectiles[i].active = false; // Mark all slots as "Free"
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
  // Iterate through the pool to find the first available slot
  for (i = 0; i < projectiles->count; i++) {
    if (!projectiles->projectiles[i].active) {
      // --- Initialize Bullet State ---
      projectiles->projectiles[i].x = x;
      projectiles->projectiles[i].y = y;
      projectiles->projectiles[i].velocityX = 0.0f; // Default horizontal speed

      // --- Velocity Logic ---
      if (direction == MOVE_UP) {
        // Player shoots UP (Negative Y)
        projectiles->projectiles[i].velocityY = -PROJECTILE_SPEED;
      } else if (direction == MOVE_DOWN) {
        // Enemies shoot DOWN (Positive Y)
        projectiles->projectiles[i].velocityY = PROJECTILE_SPEED;
      }
      // Boss Special Attacks (Diagonal)
      else if (direction == MOVE_DOWN_LEFT) {
        projectiles->projectiles[i].velocityX = -150.0f;
        projectiles->projectiles[i].velocityY = PROJECTILE_SPEED;
      } else if (direction == MOVE_DOWN_RIGHT) {
        projectiles->projectiles[i].velocityX = 150.0f;
        projectiles->projectiles[i].velocityY = PROJECTILE_SPEED;
      } else {
        projectiles->projectiles[i].velocityY = 0.0f;
      }

      // Set Hitbox dimensions
      projectiles->projectiles[i].w = PROJECTILE_WIDTH;
      projectiles->projectiles[i].h = PROJECTILE_HEIGHT;

      // Activate the bullet
      projectiles->projectiles[i].active = true;
      break; // Exit loop (spawned one bullet)
    }
  }
}

void updateProjectiles(Projectiles *projectiles, float deltaTime,
                       unsigned screenHeight) {
  if (!projectiles) {
    return;
  }

  // Update Physics for all ACTIVE bullets
  for (unsigned i = 0; i < projectiles->count; i++) {
    if (projectiles->projectiles[i].active) {

      // 1. Apply Velocity
      projectiles->projectiles[i].y +=
          projectiles->projectiles[i].velocityY * deltaTime;
      projectiles->projectiles[i].x +=
          projectiles->projectiles[i].velocityX * deltaTime;

      // 2. Boundary Check (Garbage Collection)
      // If bullet goes off-screen (Top, Bottom, Left, or Right), deactivate it.
      if (projectiles->projectiles[i].y < 0 ||
          projectiles->projectiles[i].y > screenHeight ||
          projectiles->projectiles[i].x < 0 ||
          projectiles->projectiles[i].x >
              800) { // Hardcoded 800 width assumption

        projectiles->projectiles[i].active = false; // Return to pool
      }
    }
  }
}