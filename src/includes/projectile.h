#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <stdbool.h>

#define MAX_PROJECTILES 20
#define PROJECTILE_SPEED 500.0f
#define PROJECTILE_WIDTH 5
#define PROJECTILE_HEIGHT 10

typedef enum _DirectionProjectile {
  MOVE_UP = -1,
  MOVE_DOWN = 1
} DirectionProjectile;

/**
 * @brief Structure representing a single projectile (bullet).
 */
typedef struct _Projectile {
  float x, y;
  float w, h;
  float velocityY;
  bool active;
} Projectile;

/**
 * @brief Container structure that holds the pool of projectiles.
 */
typedef struct _Projectiles {
  Projectile projectiles[MAX_PROJECTILES];
  unsigned count;
} Projectiles;

/**
 * @brief Allocates memory for the projectile pool and initializes it.
 * @return Projectiles* Pointer to the newly allocated pool, or NULL on failure.
 */
Projectiles *createProjectiles(unsigned count);

/**
 * @brief Frees the memory allocated for the projectile pool.
 * @param projectiles Pointer to the pool to free.
 */
void destroyProjectiles(Projectiles *projectiles);

/**
 * @brief Spawns a projectile from the pool.
 */
void spawnProjectile(Projectiles *projectiles, float x, float y,
                     DirectionProjectile direction);

/**
 * @brief Updates the position of all active projectiles.
 */
void updateProjectiles(Projectiles *projectiles, float deltaTime,
                       unsigned screenHeight);

#endif // PROJECTILE_H