#ifndef ENEMY_H
#define ENEMY_H

#include "projectile.h"
#include <stdbool.h>

#define ENEMY_ROWS 5
#define ENEMY_COLS 11
#define TOTAL_ENEMIES (ENEMY_ROWS * ENEMY_COLS)

#define ENEMY_WIDTH 40
#define ENEMY_HEIGHT 30
#define ENEMY_PADDING 10
#define ENEMY_START_X 50
#define ENEMY_START_Y 50
#define ENEMY_KILL_SCORE 100

// MOVEMENT SETTINGS
#define ENEMY_STEP_X 10.0f // How many pixels they jump per step
#define ENEMY_DROP_AMOUNT 20.0f

// SPEED SETTINGS (Difficulty)
#define MAX_MOVE_INTERVAL 1.0f  // Slowest speed (Start of game)
#define MIN_MOVE_INTERVAL 0.05f // Fastest speed (1 enemy left)

#define MAX_SHOOT_COOLDOWN 1.5f // Slowest shooting
#define MIN_SHOOT_COOLDOWN 0.3f // Fastest shooting

typedef struct {
  float x, y;
  unsigned width, height;
  unsigned killScore;
  bool active;
} Enemy;

typedef struct {
  bool active;
  float x, y;
  float width, height;
  int health;
  int maxHealth;
  float moveTimer;
  int direction;
} Boss;

typedef struct {
  Enemy enemies[TOTAL_ENEMIES];
  Boss boss;
  int direction;

  // NEW: Movement Timer Logic
  float moveTimer; // Accumulates deltaTime
  float
      moveInterval; // Time required to trigger a step (changes based on health)

  // NEW: Shooting Timer Logic
  float shootTimer;
  float shootCooldown; // Changes based on health

  unsigned aliveCount; // Tracks how many enemies are left

  int level;

  bool animationFrame;
} Swarm;

Swarm *createSwarm(int level);
void destroySwarm(Swarm *swarm);
void updateSwarm(Swarm *swarm, float deltaTime, unsigned screenWidth);
bool enemyAttemptShoot(Swarm *swarm, Projectiles *projectiles, float deltaTime);
bool isSwarmDestroyed(const Swarm *swarm);
void changeFrame(Swarm *swarm);

#endif