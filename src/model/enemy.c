#include "../includes/enemy.h"
#include <stdlib.h>
#include <time.h>

Swarm *createSwarm(void) {
  Swarm *s = (Swarm *)malloc(sizeof(Swarm));
  if (!s)
    return NULL;

  s->direction = 1; // 1 = Right, -1 = Left

  // Initialize Timers
  s->moveTimer = 0.0f;
  s->moveInterval = MAX_MOVE_INTERVAL;
  s->shootTimer = 0.0f;
  s->shootCooldown = MAX_SHOOT_COOLDOWN;
  s->animationFrame = false;
  s->aliveCount = TOTAL_ENEMIES;

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
      s->enemies[index].killScore = ENEMY_KILL_SCORE; // Set kill score
      index++;
    }
  }
  return s;
}

void destroySwarm(Swarm *swarm) {
  if (swarm)
    free(swarm);
}

// Helper: Counts alive enemies and updates speed
void updateSwarmSpeed(Swarm *swarm) {
  unsigned count = 0;
  for (int i = 0; i < TOTAL_ENEMIES; i++) {
    if (swarm->enemies[i].active)
      count++;
  }
  swarm->aliveCount = count;

  // Difficulty Factor: 1.0 (All alive) -> 0.0 (None alive)
  float ratio = (float)count / (float)TOTAL_ENEMIES;

  // Linear Interpolation:
  // As ratio goes down, Interval goes from MAX to MIN
  swarm->moveInterval =
      MIN_MOVE_INTERVAL + (MAX_MOVE_INTERVAL - MIN_MOVE_INTERVAL) * ratio;
  swarm->shootCooldown =
      MIN_SHOOT_COOLDOWN + (MAX_SHOOT_COOLDOWN - MIN_SHOOT_COOLDOWN) * ratio;
}

void updateSwarm(Swarm *swarm, float deltaTime, unsigned screenWidth) {
  if (!swarm)
    return;

  // 1. Recalculate Speed based on alive enemies
  updateSwarmSpeed(swarm);

  // 2. Accumulate Timer
  swarm->moveTimer += deltaTime;

  // 3. Only Move if Timer > Interval ("Step" movement)
  if (swarm->moveTimer >= swarm->moveInterval) {

    // Reset Timer
    swarm->moveTimer = 0.0f;
    changeFrame(swarm);

    // --- LOGIC: Check "Original Block" Collision ---
    // We check the VIRTUAL boundaries of the grid, even if those enemies are
    // dead. Top-Left Enemy (Index 0) defines the Left Edge. Top-Right Enemy
    // (Index ENEMY_COLS-1) defines the Right Edge.

    float leftEdgeX = swarm->enemies[0].x;
    // Index 10 is the end of the first row
    float rightEdgeX = swarm->enemies[ENEMY_COLS - 1].x + ENEMY_WIDTH;

    bool hitEdge = false;

    if (swarm->direction == 1 && rightEdgeX >= screenWidth) {
      hitEdge = true;
    } else if (swarm->direction == -1 && leftEdgeX <= 0) {
      hitEdge = true;
    }

    // --- APPLY MOVEMENT ---
    if (hitEdge) {
      // Hit Wall: Drop Down and Reverse
      swarm->direction *= -1;
      for (int i = 0; i < TOTAL_ENEMIES; i++) {
        swarm->enemies[i].y += ENEMY_DROP_AMOUNT;
        // No horizontal move on the drop frame, or small adjustment
      }
    } else {
      // No Hit: Teleport Horizontally
      float step = ENEMY_STEP_X * swarm->direction;
      for (int i = 0; i < TOTAL_ENEMIES; i++) {
        // Move EVERYONE (active and inactive) to keep grid aligned
        swarm->enemies[i].x += step;
      }
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

  // 2. Reset Timer (Using the dynamic cooldown calculated in updateSwarm)
  swarm->shootTimer = swarm->shootCooldown;

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
  return (swarm->aliveCount == 0);
}

void changeFrame(Swarm *swarm) {
  swarm->animationFrame = !swarm->animationFrame;
}