#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <stdbool.h>

/**
 * @file projectile.h
 * @brief Manages the "Bullet Hell" aspects of the game.
 * * This module implements an Object Pool for projectiles. Instead of
 * allocating and freeing memory every time a gun is fired, a fixed array of
 * `Projectile` structs is created at startup. Spawning a bullet simply involves
 * finding an inactive slot in this array and turning it on.
 */

// ==========================================
//               CONSTANTS
// ==========================================

/**
 * @brief Maximum number of simultaneous bullets on screen (Player + Enemies).
 * If the pool is full, new firing attempts are ignored.
 */
#define MAX_PROJECTILES 20

/**
 * @brief Vertical speed of the bullets in pixels per second.
 */
#define PROJECTILE_SPEED 500.0f

/**
 * @brief Width of the bullet sprite/rect in pixels.
 */
#define PROJECTILE_WIDTH 5

/**
 * @brief Height of the bullet sprite/rect in pixels.
 */
#define PROJECTILE_HEIGHT 10

// ==========================================
//               STRUCTURES
// ==========================================

/**
 * @brief Defines the trajectory of a projectile.
 */
typedef enum _DirectionProjectile {
  MOVE_UP = -1,   /**< Player bullets moving towards the top. */
  MOVE_DOWN = 1,  /**< Enemy bullets moving towards the bottom. */
  MOVE_DOWN_LEFT, /**< (Optional) Boss spread patterns. */
  MOVE_DOWN_RIGHT /**< (Optional) Boss spread patterns. */
} DirectionProjectile;

/**
 * @brief Structure representing a single projectile (bullet).
 */
typedef struct _Projectile {
  float x;         /**< Current X position. */
  float y;         /**< Current Y position. */
  float w;         /**< Width (hitbox). */
  float h;         /**< Height (hitbox). */
  float velocityY; /**< Vertical speed component. */
  float velocityX; /**< Horizontal speed component (usually 0). */
  bool active; /**< true if this bullet is currently flying; false if available.
                */
} Projectile;

/**
 * @brief Container structure that holds the pool of projectiles.
 */
typedef struct _Projectiles {
  /** @brief The fixed-size memory pool. */
  Projectile projectiles[MAX_PROJECTILES];

  /** @brief The capacity of the pool (usually MAX_PROJECTILES). */
  unsigned count;
} Projectiles;

// ==========================================
//               FUNCTIONS
// ==========================================

/**
 * @brief Allocates memory for the projectile pool and initializes it.
 * Sets all projectiles to `active = false`.
 * * @param count The size of the pool to initialize (usually MAX_PROJECTILES).
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
 * * Scans the array for the first inactive slot. If found:
 * 1. Sets position to (x, y).
 * 2. Sets velocity based on `direction` (Up for player, Down for enemies).
 * 3. Sets `active = true`.
 * * @param projectiles Pointer to the pool.
 * @param x           Starting X coordinate (center of the shooter).
 * @param y           Starting Y coordinate (nozzle of the shooter).
 * @param direction   Enum defining flight path (MOVE_UP, MOVE_DOWN, etc.).
 */
void spawnProjectile(Projectiles *projectiles, float x, float y,
                     DirectionProjectile direction);

/**
 * @brief Updates the position of all active projectiles.
 * * This function:
 * 1. Moves active bullets: `y += velocity * deltaTime`.
 * 2. Checks bounds: If a bullet leaves the screen (top or bottom),
 * it is deactivated (`active = false`) and returns to the pool.
 * * @param projectiles  Pointer to the pool.
 * @param deltaTime    Time elapsed since last frame (seconds).
 * @param screenHeight Logical height of the screen (for boundary checks).
 */
void updateProjectiles(Projectiles *projectiles, float deltaTime,
                       unsigned screenHeight);

#endif // PROJECTILE_H