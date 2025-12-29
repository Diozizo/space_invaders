#ifndef BUNKER_H
#define BUNKER_H

#include "projectile.h"
#include <stdbool.h>

/**
 * @file bunker.h
 * @brief Manages the destructible shields (bunkers) protecting the player.
 * * This module handles the creation, rendering, and collision detection
 * for the bunkers. Each bunker is composed of smaller "blocks" that can
 * be individually destroyed by projectiles from either the player or enemies.
 */

// ==========================================
//               CONSTANTS
// ==========================================

/**
 * @brief The total number of bunkers generated on the screen.
 * Standard Space Invaders usually has 4.
 */
#define BUNKER_COUNT 4

/**
 * @brief The number of rows of blocks in a single bunker.
 */
#define BUNKER_ROWS 8

/**
 * @brief The number of columns of blocks in a single bunker.
 */
#define BUNKER_COLS 10

/**
 * @brief The width and height of a single bunker block in pixels.
 * Used for rendering and collision bounding boxes.
 */
#define BLOCK_SIZE 8.0f

// ==========================================
//               STRUCTURES
// ==========================================

/**
 * @brief Represents a single destructible piece of a bunker.
 * * A bunker is made of many of these blocks. When a projectile hits a block,
 * that specific block is deactivated (destroyed), eroding the bunker.
 */
typedef struct {
  float x;     /**< Absolute X position on the screen. */
  float y;     /**< Absolute Y position on the screen. */
  bool active; /**< True if the block exists; False if it has been destroyed. */
} BunkerBlock;

/**
 * @brief Represents a single Bunker entity.
 * * A bunker acts as a shield. It is defined by a top-left position and
 * an array of blocks that constitute its shape.
 */
typedef struct {
  float x; /**< The top-left X coordinate of the entire bunker structure. */
  float y; /**< The top-left Y coordinate of the entire bunker structure. */

  /** * @brief Flattened array of blocks representing the bunker's grid.
   * Access logic usually maps (row, col) to index [row * COLS + col].
   */
  BunkerBlock blocks[BUNKER_ROWS * BUNKER_COLS];
} Bunker;

/**
 * @brief High-level manager that holds all bunkers in the game level.
 */
typedef struct {
  Bunker bunkers[BUNKER_COUNT]; /**< Array of all bunkers on screen. */
} BunkerManager;

// ==========================================
//               FUNCTIONS
// ==========================================

/**
 * @brief Allocates memory for the BunkerManager and initializes the bunkers.
 * * Calculates the spacing based on the screen width to center the bunkers
 * evenly across the screen.
 * * @param screenWidth The logical width of the game screen (used for spacing).
 * @return BunkerManager* Pointer to the newly allocated manager, or NULL on
 * failure.
 */
BunkerManager *createBunkers(unsigned screenWidth);

/**
 * @brief Frees the memory allocated for the BunkerManager.
 * * @param bm Pointer to the BunkerManager to free. Safe to pass NULL.
 */
void destroyBunkers(BunkerManager *bm);

/**
 * @brief Checks for collisions between a projectile and any active bunker
 * block.
 * * If a collision is detected:
 * 1. The specific `BunkerBlock` hit is set to inactive (destroyed).
 * 2. The function returns true, indicating the projectile should also be
 * destroyed.
 * * @param bm Pointer to the BunkerManager.
 * @param p  Pointer to the projectile to check.
 * @return true  If the projectile hit a bunker block.
 * @return false If no collision occurred.
 */
bool checkBunkerCollision(BunkerManager *bm, Projectile *p);

/**
 * @brief Restores all bunkers to their pristine state.
 * * Reactivates all blocks in every bunker. Used when restarting the game
 * or potentially when advancing to a new level (if game design dictates).
 * * @param bm          Pointer to the BunkerManager.
 * @param screenWidth The logical width of the screen (to recalculate positions
 * if needed).
 */
void resetBunkers(BunkerManager *bm, unsigned screenWidth);

#endif // BUNKER_H