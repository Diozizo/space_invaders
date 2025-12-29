#include "../../includes/physics.h"
#include "../../includes/bunker.h"
#include <stdbool.h>
#include <stdio.h>

/**
 * @brief Helper function for AABB (Axis-Aligned Bounding Box) overlap checks.
 * Returns true if two rectangles intersect.
 */
bool checkOverlap(float x1, float y1, float w1, float h1, float x2, float y2,
                  float w2, float h2) {
  // Logic: The rectangles overlap if they are NOT separated on any axis.
  return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

bool checkCollisions(Player *player, Swarm *swarm, Projectiles *projectiles,
                     ExplosionManager *explosions, BunkerManager *bunkers,
                     bool *enemyHit) {
  if (!player || !swarm || !projectiles)
    return false;

  // Reset the "Enemy Hit" sound trigger for this frame
  if (enemyHit)
    *enemyHit = false;

  // Iterate through all active projectiles (bullets)
  for (int i = 0; i < MAX_PROJECTILES; i++) {
    Projectile *p = &projectiles->projectiles[i];

    if (!p->active)
      continue; // Skip unused bullet slots

    // --- CHECK 1: PROJECTILE VS BUNKERS ---
    // Bunkers block BOTH player and enemy fire.
    // If a bullet hits a bunker, it is destroyed immediately, so we 'continue'
    // to the next bullet.
    if (bunkers && checkBunkerCollision(bunkers, p)) {
      continue;
    }

    // --- CHECK 2: PLAYER BULLETS (Moving UP) ---
    if (p->velocityY < 0) {

      // A: BOSS COLLISION (Level 2+)
      if (swarm->level == 2 && swarm->boss.active) {
        if (checkOverlap(p->x, p->y, p->w, p->h, swarm->boss.x, swarm->boss.y,
                         swarm->boss.width, swarm->boss.height)) {

          p->active = false;    // Destroy bullet
          swarm->boss.health--; // Damage Boss

          // Check for Boss Death
          if (swarm->boss.health <= 0) {
            swarm->boss.active = false;
            player->score += 1000; // Big points for Boss

            if (explosions)
              spawnExplosion(explosions, swarm->boss.x, swarm->boss.y);

            if (enemyHit)
              *enemyHit = true; // Trigger explosion sound
          }
        }
      }
      // B: STANDARD SWARM COLLISION (Level 1)
      else {
        for (int j = 0; j < TOTAL_ENEMIES; j++) {
          Enemy *e = &swarm->enemies[j];

          // Only check collision against ALIVE enemies
          if (e->active && checkOverlap(p->x, p->y, p->w, p->h, e->x, e->y,
                                        e->width, e->height)) {
            p->active = false; // Destroy bullet
            e->active = false; // Destroy enemy
            player->score += e->killScore;

            if (explosions) {
              spawnExplosion(explosions, e->x, e->y);
            }
            if (enemyHit)
              *enemyHit = true; // Trigger explosion sound

            break; // Bullet can only hit one enemy, stop checking this bullet
          }
        }
      }
    }

    // --- CHECK 3: ENEMY BULLETS (Moving DOWN) ---
    else if (p->velocityY > 0) {
      // Check vs Player
      if (checkOverlap(p->x, p->y, p->w, p->h, player->x, player->y,
                       player->width, player->height)) {

        p->active = false; // Destroy bullet

        if (player->health > 0) {
          player->health--;
          printf("Player Hit! Health: %d\n", player->health);

          if (player->health == 0)
            return true; // Return TRUE indicates Game Over (Player Died)
        }
      }
    }
  }
  return false; // Player survived this frame
}