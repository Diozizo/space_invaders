#ifndef ENEMY_H
#define ENEMY_H

#include "projectile.h"
#include <stdbool.h>

// ... existing constants ...
#define ENEMY_ROWS 5
#define ENEMY_COLS 11
#define TOTAL_ENEMIES (ENEMY_ROWS * ENEMY_COLS)

// ... existing layout constants ...
#define ENEMY_WIDTH 40
#define ENEMY_HEIGHT 30
#define ENEMY_PADDING 10
#define ENEMY_START_X 50
#define ENEMY_START_Y 50
#define ENEMY_SPEED_X 50.0f
#define ENEMY_DROP_AMOUNT 20.0f

// NEW: How fast they shoot (seconds)
#define ENEMY_SHOOT_COOLDOWN 1.5f

typedef struct {
  float x, y;
  unsigned width, height;
  bool active;
} Enemy;

typedef struct {
  Enemy enemies[TOTAL_ENEMIES];
  int direction;
  float moveSpeed;
  float shootTimer;
} Swarm;

Swarm *createSwarm(void);
void destroySwarm(Swarm *swarm);
void updateSwarm(Swarm *swarm, float deltaTime, unsigned screenWidth);

// NEW: The shooting logic
void enemyAttemptShoot(Swarm *swarm, Projectiles *projectiles, float deltaTime);

#endif