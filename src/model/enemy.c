#include "../includes/enemy.h"
#include <stdlib.h>
#include <time.h> // Required for rand()

Swarm *createSwarm(void) {
  Swarm *s = (Swarm *)malloc(sizeof(Swarm));
  if (!s)
    return NULL;

  s->direction = 1; // Start moving Right
  s->moveSpeed = ENEMY_SPEED_X;
  s->shootTimer = ENEMY_SHOOT_COOLDOWN; // Initialize the shooting timer

  int index = 0;
  for (int row = 0; row < ENEMY_ROWS; row++) {
    for (int col = 0; col < ENEMY_COLS; col++) {
      // Calculate position in the grid
      float x = ENEMY_START_X + col * (ENEMY_WIDTH + ENEMY_PADDING);
      float y = ENEMY_START_Y + row * (ENEMY_HEIGHT + ENEMY_PADDING);

      s->enemies[index].x = x;
      s->enemies[index].y = y;
      s->enemies[index].width = ENEMY_WIDTH;
      s->enemies[index].height = ENEMY_HEIGHT;
      s->enemies[index].active = true;

      index++;
    }
  }
  return s;
}

void destroySwarm(Swarm *swarm) {
  if (swarm)
    free(swarm);
}

void updateSwarm(Swarm *swarm, float deltaTime, unsigned screenWidth) {
  if (!swarm)
    return;

  bool hitEdge = false;

  // 1. Check if ANY alive enemy has hit the edge
  for (int i = 0; i < TOTAL_ENEMIES; i++) {
    if (!swarm->enemies[i].active)
      continue;

    // Check Right Edge (only if moving Right)
    if (swarm->direction == 1 &&
        swarm->enemies[i].x + ENEMY_WIDTH >= screenWidth) {
      hitEdge = true;
      break;
    }
    // Check Left Edge (only if moving Left)
    if (swarm->direction == -1 && swarm->enemies[i].x <= 0) {
      hitEdge = true;
      break;
    }
  }

  // 2. Apply Movement
  if (hitEdge) {
    // Drop down and reverse
    swarm->direction *= -1; // Flip direction (1 becomes -1)

    for (int i = 0; i < TOTAL_ENEMIES; i++) {
      swarm->enemies[i].y += ENEMY_DROP_AMOUNT;
      // Nudge them slightly away from the wall to prevent getting stuck
      swarm->enemies[i].x += (swarm->direction * 2.0f);
    }
  } else {
    // Normal horizontal movement
    for (int i = 0; i < TOTAL_ENEMIES; i++) {
      if (swarm->enemies[i].active) {
        swarm->enemies[i].x +=
            (swarm->moveSpeed * swarm->direction) * deltaTime;
      }
    }
  }
}

void enemyAttemptShoot(Swarm *swarm, Projectiles *projectiles,
                       float deltaTime) {
  if (!swarm || !projectiles)
    return;

  // 1. Update Timer
  if (swarm->shootTimer > 0) {
    swarm->shootTimer -= deltaTime;
    return; // Not ready to shoot yet
  }

  // 2. Reset Timer
  swarm->shootTimer = ENEMY_SHOOT_COOLDOWN;

  // 3. Pick a random column to start searching
  // We try 'attempts' times to find a column with living enemies
  // to prevent the game from doing nothing if we pick an empty column.
  int attempts = ENEMY_COLS;
  int startCol = rand() % ENEMY_COLS;

  for (int i = 0; i < attempts; i++) {
    // Wrap around logic: ensures we check all columns if needed
    int col = (startCol + i) % ENEMY_COLS;

    // 4. Find bottom-most active enemy in this column
    // We start from the bottom Row (ENEMY_ROWS - 1) and go UP
    for (int row = ENEMY_ROWS - 1; row >= 0; row--) {
      int index =
          row * ENEMY_COLS + col; // Convert Grid (row,col) to Array Index

      if (swarm->enemies[index].active) {
        // FOUND A SHOOTER!
        Enemy *shooter = &swarm->enemies[index];

        // Calculate position: Center X, Bottom Y of the enemy
        float bulletX =
            shooter->x + (shooter->width / 2.0f) - (PROJECTILE_WIDTH / 2.0f);
        float bulletY = shooter->y + shooter->height;

        // Spawn with MOVE_DOWN
        spawnProjectile(projectiles, bulletX, bulletY, MOVE_DOWN);
        return; // Shot fired, stop logic for this frame
      }
    }
  }
}