#include "../includes/enemy.h"
#include <math.h> // For any math needs
#include <stdlib.h>
#include <time.h>

Swarm *createSwarm(int level) {
  Swarm *s = (Swarm *)calloc(1, sizeof(Swarm));
  if (!s)
    return NULL;

  s->level = level; // <--- IMPORTANT: Save the level!
  s->shootTimer = 0.0f;

  if (level == 1) {
    s->direction = 1; // 1 = Right, -1 = Left

    // Initialize Timers
    s->moveTimer = 0.0f;
    s->moveInterval = MAX_MOVE_INTERVAL;
    s->shootCooldown = MAX_SHOOT_COOLDOWN;
    s->animationFrame = false;
    s->aliveCount = TOTAL_ENEMIES;

    // Disable Boss
    s->boss.active = false;

    int index = 0;
    for (int row = 0; row < ENEMY_ROWS; row++) {
      for (int col = 0; col < ENEMY_COLS; col++) {
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
  } else if (level == 2) {
    // Disable normal enemies
    for (int i = 0; i < TOTAL_ENEMIES; i++)
      s->enemies[i].active = false;

    // Init Boss
    s->boss.active = true;
    s->boss.width = 64.0f;
    s->boss.height = 64.0f;
    s->boss.x = 400.0f - 32.0f; // Center
    s->boss.y = 80.0f;
    s->boss.health = 20; // 20 hits to kill
    s->boss.maxHealth = 20;
    s->boss.direction = 1;
    s->shootCooldown = 0.5f; // Fixed shoot interval for boss
  }
  return s;
}

void destroySwarm(Swarm *swarm) {
  if (swarm) {
    free(swarm);
  }
}

// Helper: Counts alive enemies and updates speed
void updateSwarmSpeed(Swarm *swarm) {
  unsigned count = 0;
  for (int i = 0; i < TOTAL_ENEMIES; i++) {
    if (swarm->enemies[i].active)
      count++;
  }
  swarm->aliveCount = count;

  float ratio = (float)count / (float)TOTAL_ENEMIES;

  swarm->moveInterval =
      MIN_MOVE_INTERVAL + (MAX_MOVE_INTERVAL - MIN_MOVE_INTERVAL) * ratio;
  swarm->shootCooldown =
      MIN_SHOOT_COOLDOWN + (MAX_SHOOT_COOLDOWN - MIN_SHOOT_COOLDOWN) * ratio;
}

void updateSwarm(Swarm *swarm, float deltaTime, unsigned screenWidth) {
  if (!swarm)
    return;

  if (swarm->level == 1) {
    updateSwarmSpeed(swarm);
    swarm->moveTimer += deltaTime;

    if (swarm->moveTimer >= swarm->moveInterval) {
      swarm->moveTimer = 0.0f;
      changeFrame(swarm);

      float leftEdgeX = swarm->enemies[0].x;
      float rightEdgeX = swarm->enemies[ENEMY_COLS - 1].x + ENEMY_WIDTH;

      bool hitEdge = false;

      if (swarm->direction == 1 && rightEdgeX >= screenWidth) {
        hitEdge = true;
      } else if (swarm->direction == -1 && leftEdgeX <= 0) {
        hitEdge = true;
      }

      if (hitEdge) {
        swarm->direction *= -1;
        for (int i = 0; i < TOTAL_ENEMIES; i++) {
          swarm->enemies[i].y += ENEMY_DROP_AMOUNT;
        }
      } else {
        float step = ENEMY_STEP_X * swarm->direction;
        for (int i = 0; i < TOTAL_ENEMIES; i++) {
          swarm->enemies[i].x += step;
        }
      }
    }
  }
  // --- BOSS UPDATE ---
  else if (swarm->level == 2 && swarm->boss.active) {
    // Fixed: Changed 's' to 'swarm'
    float speed = 150.0f;
    swarm->boss.x += swarm->boss.direction * speed * deltaTime;

    // Bounce off walls
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

  // 1. Update Timer
  if (swarm->shootTimer > 0) {
    swarm->shootTimer -= deltaTime;
    return false;
  }

  // 2. Reset Timer
  swarm->shootTimer = swarm->shootCooldown;

  // --- BOSS SHOOTING ---
  if (swarm->level == 2 && swarm->boss.active) {
    float bulletX =
        swarm->boss.x + (swarm->boss.width / 2.0f) - (PROJECTILE_WIDTH / 2.0f);
    float bulletY = swarm->boss.y + swarm->boss.height;

    // SHOT 1: Straight Down
    spawnProjectile(projectiles, bulletX, bulletY, MOVE_DOWN);

    // SHOT 2: Diagonal Left
    spawnProjectile(projectiles, bulletX, bulletY, MOVE_DOWN_LEFT);

    // SHOT 3: Diagonal Right
    spawnProjectile(projectiles, bulletX, bulletY, MOVE_DOWN_RIGHT);

    return true;
  }

  // --- NORMAL ENEMY SHOOTING ---
  // 3. Pick a random column
  int attempts = ENEMY_COLS;
  int startCol = rand() % ENEMY_COLS;

  for (int i = 0; i < attempts; i++) {
    int col = (startCol + i) % ENEMY_COLS;

    // 4. Find bottom-most active enemy
    for (int row = ENEMY_ROWS - 1; row >= 0; row--) {
      int index = row * ENEMY_COLS + col;

      if (swarm->enemies[index].active) {
        Enemy *shooter = &swarm->enemies[index];

        float bulletX =
            shooter->x + (shooter->width / 2.0f) - (PROJECTILE_WIDTH / 2.0f);
        float bulletY = shooter->y + shooter->height;

        spawnProjectile(projectiles, bulletX, bulletY, MOVE_DOWN);
        return true;
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