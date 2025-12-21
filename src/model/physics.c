#include "../includes/physics.h"
#include <stdio.h> // For debug prints

// Helper function: Returns true if two rectangles overlap
bool checkOverlap(float x1, float y1, float w1, float h1, float x2, float y2,
                  float w2, float h2) {
  return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

bool checkCollisions(Player *player, Swarm *swarm, Projectiles *projectiles) {
  if (!player || !swarm || !projectiles)
    return false;

  // Iterate through all projectiles
  for (int i = 0; i < MAX_PROJECTILES; i++) {
    Projectile *p = &projectiles->projectiles[i];

    if (!p->active)
      continue;

    // --- SCENARIO 1: Player Bullet (Moving UP) vs Enemy ---
    if (p->velocityY < 0) {
      for (int j = 0; j < TOTAL_ENEMIES; j++) {
        Enemy *e = &swarm->enemies[j];

        if (e->active && checkOverlap(p->x, p->y, p->w, p->h, e->x, e->y,
                                      e->width, e->height)) {
          // HIT!
          p->active = false; // Destroy Bullet
          e->active = false; // Destroy Enemy
          // Optional: Add score here later
          break; // Bullet can only hit one enemy
        }
      }
    }

    // --- SCENARIO 2: Enemy Bullet (Moving DOWN) vs Player ---
    else if (p->velocityY > 0) {
      if (checkOverlap(p->x, p->y, p->w, p->h, player->x, player->y,
                       player->width, player->height)) {
        // HIT!
        p->active = false; // Destroy Bullet

        if (player->health > 0) {
          player->health--;
          printf("Player Hit! Health: %d\n", player->health);

          if (player->health == 0) {
            return true; // Signal that player died
          }
        }
      }
    }
  }

  return false; // Player is still alive
}