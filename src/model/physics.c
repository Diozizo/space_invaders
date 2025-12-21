#include "../includes/physics.h"
#include <stdio.h>

// ... checkOverlap helper function remains the same ...
bool checkOverlap(float x1, float y1, float w1, float h1, float x2, float y2,
                  float w2, float h2) {
  return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

// Updated signature
bool checkCollisions(Player *player, Swarm *swarm, Projectiles *projectiles,
                     ExplosionManager *explosions) {
  if (!player || !swarm || !projectiles)
    return false;

  for (int i = 0; i < MAX_PROJECTILES; i++) {
    Projectile *p = &projectiles->projectiles[i];

    if (!p->active)
      continue;

    // 1. Player Bullet (UP) vs Enemy
    if (p->velocityY < 0) {
      for (int j = 0; j < TOTAL_ENEMIES; j++) {
        Enemy *e = &swarm->enemies[j];

        if (e->active && checkOverlap(p->x, p->y, p->w, p->h, e->x, e->y,
                                      e->width, e->height)) {
          // HIT!
          p->active = false;
          e->active = false;

          // --- NEW: Spawn Explosion ---
          if (explosions) {
            spawnExplosion(explosions, e->x, e->y);
          }

          break;
        }
      }
    }

    // 2. Enemy Bullet (DOWN) vs Player
    else if (p->velocityY > 0) {
      if (checkOverlap(p->x, p->y, p->w, p->h, player->x, player->y,
                       player->width, player->height)) {
        p->active = false;
        if (player->health > 0) {
          player->health--;
          printf("Player Hit! Health: %d\n", player->health);
          if (player->health == 0)
            return true;
        }
      }
    }
  }
  return false;
}