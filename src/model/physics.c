#include "../includes/physics.h"
#include "../includes/bunker.h"
#include <stdbool.h>
#include <stdio.h>

bool checkOverlap(float x1, float y1, float w1, float h1, float x2, float y2,
                  float w2, float h2) {
  return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

bool checkCollisions(Player *player, Swarm *swarm, Projectiles *projectiles,
                     ExplosionManager *explosions, BunkerManager *bunkers,
                     bool *enemyHit) {
  if (!player || !swarm || !projectiles)
    return false;

  // Initialize flag
  if (enemyHit)
    *enemyHit = false;

  for (int i = 0; i < MAX_PROJECTILES; i++) {
    Projectile *p = &projectiles->projectiles[i];

    if (!p->active)
      continue;

    if (bunkers && checkBunkerCollision(bunkers, p)) {
      continue;
    }

    // 1. Player Bullet (UP)
    if (p->velocityY < 0) {

      // --- BOSS COLLISION CHECK ---
      if (swarm->level == 2 && swarm->boss.active) {
        if (checkOverlap(p->x, p->y, p->w, p->h, swarm->boss.x, swarm->boss.y,
                         swarm->boss.width, swarm->boss.height)) {

          p->active = false;
          swarm->boss.health--; // Damage Boss

          if (swarm->boss.health <= 0) {
            swarm->boss.active = false;
            player->score += 1000;
            if (explosions)
              spawnExplosion(explosions, swarm->boss.x, swarm->boss.y);
            if (enemyHit)
              *enemyHit = true; // Play explosion sound
          } else {
            // Optional: Play a smaller "hit" sound here if you had one
          }
        }
      }
      // --- NORMAL ENEMY COLLISION CHECK ---
      else {
        for (int j = 0; j < TOTAL_ENEMIES; j++) {
          Enemy *e = &swarm->enemies[j];

          if (e->active && checkOverlap(p->x, p->y, p->w, p->h, e->x, e->y,
                                        e->width, e->height)) {
            p->active = false;
            e->active = false;
            player->score += e->killScore;
            if (explosions) {
              spawnExplosion(explosions, e->x, e->y);
            }
            if (enemyHit)
              *enemyHit = true;
            break;
          }
        }
      }
    }

    // 2. Enemy/Boss Bullet (DOWN) vs Player
    else if (p->velocityY > 0) {
      if (checkOverlap(p->x, p->y, p->w, p->h, player->x, player->y,
                       player->width, player->height)) {
        p->active = false;
        if (player->health > 0) {
          player->health--;
          printf("Player Hit! Health: %d\n", player->health);
          if (player->health == 0)
            return true; // Game Over
        }
      }
    }
  }
  return false;
}