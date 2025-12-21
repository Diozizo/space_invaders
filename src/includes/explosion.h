#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <stdbool.h>

#define MAX_EXPLOSIONS 10
#define EXPLOSION_DURATION 0.3f // How long the explosion lasts (seconds)
#define EXPLOSION_SIZE 40.0f

typedef struct {
  float x, y;
  float timer;      // Counts down from DURATION to 0
  int currentFrame; // 0, 1, or 2 (based on timer)
  bool active;
} Explosion;

typedef struct {
  Explosion explosions[MAX_EXPLOSIONS];
} ExplosionManager;

ExplosionManager *createExplosionManager(void);
void destroyExplosionManager(ExplosionManager *em);

// Call this every frame to animate explosions
void updateExplosions(ExplosionManager *em, float deltaTime);

// Call this when an enemy dies
void spawnExplosion(ExplosionManager *em, float x, float y);

#endif