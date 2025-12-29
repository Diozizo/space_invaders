#ifndef ENEMY_H
#define ENEMY_H

#include "projectile.h"
#include <stdbool.h>

/**
 * @file enemy.h
 * @brief Manages the fleet of alien invaders (The Swarm) and the Boss.
 * * This module handles the "Space Invaders" logic:
 * - A grid of enemies moving in unison.
 * - Classic movement pattern (Step Right -> Hit Edge -> Drop Down -> Step
 * Left).
 * - Speed increases as fewer enemies remain.
 * - Random enemy shooting.
 * - Boss encounters at specific levels.
 */

// ==========================================
//             GRID & DIMENSIONS
// ==========================================

/** @brief Number of rows in the enemy grid. */
#define ENEMY_ROWS 5

/** @brief Number of enemies per row. */
#define ENEMY_COLS 11

/** @brief Total max capacity of the swarm. */
#define TOTAL_ENEMIES (ENEMY_ROWS * ENEMY_COLS)

/** @brief Width of a single standard enemy in pixels. */
#define ENEMY_WIDTH 40

/** @brief Height of a single standard enemy in pixels. */
#define ENEMY_HEIGHT 30

/** @brief Horizontal/Vertical gap between enemies in the grid. */
#define ENEMY_PADDING 10

/** @brief Starting X position for the top-left enemy. */
#define ENEMY_START_X 50

/** @brief Starting Y position for the top-left enemy. */
#define ENEMY_START_Y 50

/** @brief Score awarded to the player for destroying one enemy. */
#define ENEMY_KILL_SCORE 100

// ==========================================
//            MOVEMENT & DIFFICULTY
// ==========================================

/** @brief Horizontal pixels moved per "tick" (step). */
#define ENEMY_STEP_X 10.0f

/** @brief Vertical pixels dropped when the swarm hits the screen edge. */
#define ENEMY_DROP_AMOUNT 20.0f

// --- Speed Scaling (Difficulty) ---
/** @brief Maximum time between steps (Start of game, full swarm). Slowest. */
#define MAX_MOVE_INTERVAL 1.0f

/** @brief Minimum time between steps (One enemy left). Fastest. */
#define MIN_MOVE_INTERVAL 0.05f

/** @brief Maximum time between random enemy shots (Low aggression). */
#define MAX_SHOOT_COOLDOWN 1.5f

/** @brief Minimum time between random enemy shots (High aggression). */
#define MIN_SHOOT_COOLDOWN 0.3f

// ==========================================
//               STRUCTURES
// ==========================================

/**
 * @brief Represents a single standard Alien Invader.
 */
typedef struct {
  float x;            /**< Current X position. */
  float y;            /**< Current Y position. */
  unsigned width;     /**< Width for collision detection. */
  unsigned height;    /**< Height for collision detection. */
  unsigned killScore; /**< Score value of this specific unit. */
  bool active;        /**< true if alive, false if destroyed. */
} Enemy;

/**
 * @brief Represents the Boss enemy (Mother Ship).
 * Appears in specific levels (e.g., Level 2). moves independently.
 */
typedef struct {
  bool active;     /**< true if the boss is currently fighting. */
  float x;         /**< Current X position. */
  float y;         /**< Current Y position. */
  float width;     /**< Boss width. */
  float height;    /**< Boss height. */
  int health;      /**< Current HP. */
  int maxHealth;   /**< Starting HP (for health bar rendering). */
  float moveTimer; /**< Internal timer for movement updates. */
  int direction;   /**< 1 for Right, -1 for Left. */
} Boss;

/**
 * @brief The Swarm Manager.
 * * Controls the collective behavior of all enemies. Unlike modern games where
 * each entity thinks independently, Space Invaders treats the swarm as a
 * single entity that moves in steps.
 */
typedef struct {
  /** @brief Static array of all standard enemies. */
  Enemy enemies[TOTAL_ENEMIES];

  /** @brief The Boss entity associated with this swarm level. */
  Boss boss;

  /** @brief Current direction of the Swarm: 1 (Right) or -1 (Left). */
  int direction;

  // --- Movement Timer Logic ---
  /** @brief Accumulates deltaTime. When > moveInterval, the swarm steps. */
  float moveTimer;

  /** * @brief Threshold for moveTimer.
   * Decreases linearly as aliveCount decreases, making the swarm faster.
   */
  float moveInterval;

  // --- Shooting Timer Logic ---
  float shootTimer;    /**< Accumulates deltaTime for shooting. */
  float shootCooldown; /**< Time required before the next random shot. */

  unsigned aliveCount; /**< Number of active enemies remaining. */

  int level; /**< Current difficulty/wave level. */

  /** @brief Toggle for animation (arms up / arms down). Flips every step. */
  bool animationFrame;
} Swarm;

// ==========================================
//               FUNCTIONS
// ==========================================

/**
 * @brief Allocates and initializes the Swarm.
 * Sets up the grid layout, difficulty settings, and optional Boss based on
 * level.
 * @param level Current game level (1 = Standard, 2 = Boss, etc.).
 * @return Swarm* Pointer to the new Swarm object.
 */
Swarm *createSwarm(int level);

/**
 * @brief Frees the memory allocated for the Swarm.
 * @param swarm Pointer to the Swarm to free.
 */
void destroySwarm(Swarm *swarm);

/**
 * @brief Updates the Swarm's position and state.
 * * Handles:
 * 1. Accumulating timers.
 * 2. Moving the entire grid horizontally if the timer triggers.
 * 3. Detecting screen edges and dropping the swarm down.
 * 4. Updating the Boss (if active).
 * * @param swarm       Pointer to the Swarm.
 * @param deltaTime   Time elapsed since last frame.
 * @param screenWidth Logical width of the screen (for edge detection).
 */
void updateSwarm(Swarm *swarm, float deltaTime, unsigned screenWidth);

/**
 * @brief Attempts to make a random enemy fire a projectile.
 * * Logic:
 * - Checks if `shootTimer` > `shootCooldown`.
 * - If yes, picks a random column.
 * - Finds the bottom-most active enemy in that column.
 * - Spawns a projectile from that enemy.
 * * @param swarm       Pointer to the Swarm.
 * @param projectiles Pointer to the Projectile pool manager.
 * @param deltaTime   Time elapsed since last frame.
 * @return true if a shot was fired, false otherwise.
 */
bool enemyAttemptShoot(Swarm *swarm, Projectiles *projectiles, float deltaTime);

/**
 * @brief Checks if the level is cleared.
 * @param swarm Pointer to the Swarm.
 * @return true If all enemies (and Boss if applicable) are destroyed.
 */
bool isSwarmDestroyed(const Swarm *swarm);

/**
 * @brief Manually toggles the animation frame (Visuals only).
 * Usually called automatically inside updateSwarm.
 * @param swarm Pointer to the Swarm.
 */
void changeFrame(Swarm *swarm);

#endif // ENEMY_H