#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <stdbool.h>

/**
 * @file explosion.h
 * @brief Manages visual explosion effects when entities are destroyed.
 * * This module implements a simple particle-like system for handling
 * temporary visual effects. It uses a "pool" of explosion objects
 * to avoid constant memory allocation/deallocation during gameplay.
 */

// ==========================================
//               CONSTANTS
// ==========================================

/**
 * @brief Maximum number of simultaneous explosions allowed on screen.
 * If more explosions occur than this limit, the new requests are ignored.
 * A pool size of 10 is usually sufficient for standard gameplay.
 */
#define MAX_EXPLOSIONS 10

/**
 * @brief The total lifespan of an explosion animation in seconds.
 */
#define EXPLOSION_DURATION 0.3f

/**
 * @brief The width/height of the explosion sprite in pixels.
 */
#define EXPLOSION_SIZE 40.0f

// ==========================================
//               STRUCTURES
// ==========================================

/**
 * @brief Represents a single explosion instance.
 */
typedef struct {
  float x; /**< X position of the explosion center. */
  float y; /**< Y position of the explosion center. */

  /** * @brief Countdown timer for the effect.
   * Starts at EXPLOSION_DURATION and decrements by deltaTime.
   * When it reaches <= 0, the explosion is deactivated.
   */
  float timer;

  /**
   * @brief The current sprite frame to render.
   * Typically calculated as: 0 (start), 1 (middle), 2 (end).
   */
  int currentFrame;

  bool active; /**< true if this slot is currently playing an animation. */
} Explosion;

/**
 * @brief Manager that holds the pool of explosion objects.
 */
typedef struct {
  /** @brief Fixed-size array of explosion objects (Object Pool pattern). */
  Explosion explosions[MAX_EXPLOSIONS];
} ExplosionManager;

// ==========================================
//               FUNCTIONS
// ==========================================

/**
 * @brief Allocates and initializes the ExplosionManager.
 * Sets all explosion slots to inactive.
 * @return ExplosionManager* Pointer to the new manager.
 */
ExplosionManager *createExplosionManager(void);

/**
 * @brief Frees the memory allocated for the ExplosionManager.
 * @param em Pointer to the manager to free.
 */
void destroyExplosionManager(ExplosionManager *em);

/**
 * @brief Updates the state of all active explosions.
 * * This function:
 * 1. Decrements the timer of active explosions by `deltaTime`.
 * 2. Updates `currentFrame` based on how much time is left.
 * 3. Deactivates explosions whose timer has reached 0.
 * * @param em        Pointer to the ExplosionManager.
 * @param deltaTime Time elapsed since the last frame (seconds).
 */
void updateExplosions(ExplosionManager *em, float deltaTime);

/**
 * @brief Spawns a new explosion at the specified coordinates.
 * * Scans the pool for the first available (inactive) slot.
 * If a slot is found, it initializes it with the given coordinates
 * and resets its timer. If the pool is full, the request is ignored.
 * * @param em Pointer to the ExplosionManager.
 * @param x  X coordinate for the explosion.
 * @param y  Y coordinate for the explosion.
 */
void spawnExplosion(ExplosionManager *em, float x, float y);

#endif // EXPLOSION_H