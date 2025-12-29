#include "../../includes/enemy.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

Swarm *createSwarm(int level) {
  Swarm *s = (Swarm *)calloc(1, sizeof(Swarm));
  if (!s)
    return NULL;

  s->level = level;
  s->shootTimer = 0.0f;

  // --- LEVEL 1: STANDARD SWARM ---
  if (level == 1) {
    s->direction = 1; // Start moving Right

    // Initialize Difficulty Timers
    s->moveTimer = 0.0f;
    s->moveInterval = MAX_MOVE_INTERVAL; // Start slow
    s->shootCooldown = MAX_SHOOT_COOLDOWN;
    s->animationFrame = false;
    s->aliveCount = TOTAL_ENEMIES;

    // Ensure Boss is disabled for Level 1
    s->boss.active = false;

    // Grid Generation Loop
    int index = 0;
    for (int row = 0; row < ENEMY_ROWS; row++) {
      for (int col = 0; col < ENEMY_COLS; col++) {
        // Calculate grid position with padding
        float x = ENEMY_START_X + col * (ENEMY_WIDTH + ENEMY_PADDING);
        float y = ENEMY_START_Y + row * (ENEMY_HEIGHT + ENEMY_PADDING);

        s->enemies[index].x = x;
        s->enemies[index].y = y;
        s->enemies[index].width = ENEMY_WIDTH;
        s->enemies[index].height = ENEMY_HEIGHT;
        s->enemies[index].active = true;
        s->enemies[index].killScore = ENEMY_KILL_SCORE;
        index++;
      }
    }
  }
  // --- LEVEL 2: BOSS BATTLE ---
  else if (level == 2) {
    // Disable all standard swarm enemies
    for (int i = 0; i < TOTAL_ENEMIES; i++)
      s->enemies[i].active = false;

    // Initialize Boss Entity
    s->boss.active = true;
    s->boss.width = 64.0f;
    s->boss.height = 64.0f;
    s->boss.x = 400.0f - 32.0f; // Start Center
    s->boss.y = 80.0f;          // Start Top
    s->boss.health = 20;        // HP
    s->boss.maxHealth = 20;
    s->boss.direction = 1;   // Moving Right
    s->shootCooldown = 0.5f; // Boss shoots faster and consistently
  }
  return s;
}

void destroySwarm(Swarm *swarm) {
  if (swarm) {
    free(swarm);
  }
}

/**
 * @brief Recalculates swarm speed based on remaining enemies.
 * Linearly interpolates between MAX_MOVE_INTERVAL (slow) and MIN_MOVE_INTERVAL
 * (fast).
 */
void updateSwarmSpeed(Swarm *swarm) {
  unsigned count = 0;
  for (int i = 0; i < TOTAL_ENEMIES; i++) {
    if (swarm->enemies[i].active)
      count++;
  }
  swarm->aliveCount = count;

  // Ratio: 1.0 (Full Swarm) -> near 0.0 (One Enemy Left)
  float ratio = (float)count / (float)TOTAL_ENEMIES;

  // Lerp (Linear Interpolation) Formula
  swarm->moveInterval =
      MIN_MOVE_INTERVAL + (MAX_MOVE_INTERVAL - MIN_MOVE_INTERVAL) * ratio;

  // Also speed up shooting rate
  swarm->shootCooldown =
      MIN_SHOOT_COOLDOWN + (MAX_SHOOT_COOLDOWN - MIN_SHOOT_COOLDOWN) * ratio;
}

void updateSwarm(Swarm *swarm, float deltaTime, unsigned screenWidth) {
  if (!swarm)
    return;

  // --- SWARM BEHAVIOR (LEVEL 1) ---
  if (swarm->level == 1) {
    updateSwarmSpeed(swarm);

    // Accumulate time for the next "Step"
    swarm->moveTimer += deltaTime;

    // If it's time to move:
    if (swarm->moveTimer >= swarm->moveInterval) {
      swarm->moveTimer = 0.0f;
      changeFrame(swarm); // Toggle animation (Arms Up/Down)

      // 1. Check current edges of the swarm (using the first and last column
      // logic is naive but efficient) Note: A more robust check finds the
      // min/max X of *active* enemies. For this implementation, we assume the
      // outer bounds are roughly static relative to grid index. (Improving this
      // would require iterating all active enemies to find the true bounding
      // box).

      float leftEdgeX = swarm->enemies[0].x;
      float rightEdgeX = swarm->enemies[ENEMY_COLS - 1].x + ENEMY_WIDTH;
      // Note: This edge logic works best if the swarm stays rectangular.
      // If the player kills the edge columns first, the swarm might "overshoot"
      // visually.

      bool hitEdge = false;

      // Check Right Wall
      if (swarm->direction == 1 && rightEdgeX >= screenWidth) {
        hitEdge = true;
      }
      // Check Left Wall
      else if (swarm->direction == -1 && leftEdgeX <= 0) {
        hitEdge = true;
      }

      if (hitEdge) {
        // HIT WALL: Reverse direction and Drop Down
        swarm->direction *= -1;
        for (int i = 0; i < TOTAL_ENEMIES; i++) {
          swarm->enemies[i].y += ENEMY_DROP_AMOUNT;
        }
      } else {
        // NO WALL: Just move sideways
        float step = ENEMY_STEP_X * swarm->direction;
        for (int i = 0; i < TOTAL_ENEMIES; i++) {
          swarm->enemies[i].x += step;
        }
      }
    }
  }
  // --- BOSS BEHAVIOR (LEVEL 2) ---
  else if (swarm->level == 2 && swarm->boss.active) {
    float speed = 150.0f;
    // Continuous movement (float physics), not stepped
    swarm->boss.x += swarm->boss.direction * speed * deltaTime;

    // Simple Bounce Logic
    if (swarm->boss.x <= 0) {
      swarm->boss.x = 0;
      swarm->boss.direction = 1;
    } else if (swarm->boss.x + swarm->boss.width >= screenWidth) {
      swarm->boss.x = screenWidth - swarm->boss.width;
      swarm->boss.direction = -1;
    }
  }
}

bool enemyAttemptShoot(Swarm *swarm, Projectiles *projectiles,
                       float deltaTime) {
  if (!swarm || !projectiles)
    return false;

  // 1. Update Cooldown Timer
  if (swarm->shootTimer > 0) {
    swarm->shootTimer -= deltaTime;
    return false;
  }

  // 2. Reset Timer (Reload)
  swarm->shootTimer = swarm->shootCooldown;

  // --- BOSS SHOOTING PATTERN ---
  if (swarm->level == 2 && swarm->boss.active) {
    float bulletX =
        swarm->boss.x + (swarm->boss.width / 2.0f) - (PROJECTILE_WIDTH / 2.0f);
    float bulletY = swarm->boss.y + swarm->boss.height;

    // Boss fires a "Spread Shot" (3 bullets)
    spawnProjectile(projectiles, bulletX, bulletY, MOVE_DOWN);
    spawnProjectile(projectiles, bulletX, bulletY, MOVE_DOWN_LEFT);
    spawnProjectile(projectiles, bulletX, bulletY, MOVE_DOWN_RIGHT);

    return true;
  }

  // --- SWARM SHOOTING STRATEGY ---
  // Goal: Pick a random column, find the bottom-most enemy, and shoot.

  int attempts = ENEMY_COLS;
  int startCol = rand() % ENEMY_COLS; // Random starting column

  for (int i = 0; i < attempts; i++) {
    // Wrap around columns if the chosen one is empty
    int col = (startCol + i) % ENEMY_COLS;

    // Search from Bottom Row -> Up
    for (int row = ENEMY_ROWS - 1; row >= 0; row--) {
      int index = row * ENEMY_COLS + col;

      if (swarm->enemies[index].active) {
        Enemy *shooter = &swarm->enemies[index];

        float bulletX =
            shooter->x + (shooter->width / 2.0f) - (PROJECTILE_WIDTH / 2.0f);
        float bulletY = shooter->y + shooter->height;

        spawnProjectile(projectiles, bulletX, bulletY, MOVE_DOWN);
        return true; // Shot fired, exit function
      }
    }
  }
  return false;
}

bool isSwarmDestroyed(const Swarm *swarm) {
  if (!swarm)
    return true;

  if (swarm->level == 2) {
    return !swarm->boss.active;
  }

  return (swarm->aliveCount == 0);
}

void changeFrame(Swarm *swarm) {
  swarm->animationFrame = !swarm->animationFrame;
}