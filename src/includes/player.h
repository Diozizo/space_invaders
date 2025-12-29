#ifndef PLAYER_H
#define PLAYER_H

#include "projectile.h"
#include <stdbool.h>

/**
 * @file player.h
 * @brief Manages the Player entity (The Tank/Spaceship).
 *
 * This module handles the player's state, physics, input processing,
 * and weapon systems. It ensures the player stays within screen bounds
 * and adheres to firing rate limits.
 */

// ==========================================
//               CONSTANTS
// ==========================================

/**
 * @brief The movement speed of the player in pixels per second.
 * Used for calculating smooth movement: position += speed * deltaTime.
 */
#define PLAYER_SPEED 300.0f

/**
 * @brief Time in seconds required between two shots (Fire Rate).
 */
#define PLAYER_SHOOT_COOLDOWN 0.3f

/**
 * @brief The fixed Y position of the player (vertical lock).
 * @note Standard screen height is usually 600px. A value of 200.0f
 * puts the player in the top half. For a classic feel, consider 500.0f or
 * 550.0f.
 */
#define Y_AXIS 550.0f

/**
 * @brief The initial number of lives/health the player starts with.
 */
#define HEALTH 3

// ==========================================
//               STRUCTURES
// ==========================================

/**
 * @brief Enum representing the input state for horizontal movement.
 */
typedef enum {
  MOVE_NONE = 0,  /**< No keys pressed (friction/stop). */
  MOVE_LEFT = -1, /**< Moving towards 0 (Left). */
  MOVE_RIGHT = 1  /**< Moving towards Screen Width (Right). */
} Direction;

/**
 * @brief Structure representing the Player entity.
 */
typedef struct _player {
  // --- Physics & Transform ---
  float x; /**< Horizontal position (float for smooth sub-pixel physics). */
  float y; /**< Vertical position (fixed lock). */
  unsigned height; /**< Sprite height in pixels (hitbox). */
  unsigned width;  /**< Sprite width in pixels (hitbox). */
  float velocityX; /**< Current horizontal speed vector (pixels/sec). */

  // --- Gameplay State ---
  float shootTimer; /**< Countdown timer for reloading. 0.0f means Ready. */
  unsigned health;  /**< Current lives remaining. Game Over if 0. */
  unsigned score;   /**< Current score accumulated in this session. */

  // --- Animation State ---
  float animTimer; /**< Timer to toggle sprite frames. */
  int animFrame;   /**< Current frame index (for sprite sheet rendering). */
  int animDir;     /**< Direction of animation playback. */
} Player;

// ==========================================
//               FUNCTIONS
// ==========================================

/**
 * @brief Allocates memory for a new Player and initializes its values.
 *
 * @param xAxis  The starting X coordinate (usually screenWidth / 2).
 * @param height The height of the player sprite (e.g., 32).
 * @param width  The width of the player sprite (e.g., 32).
 * @return Player* Pointer to the newly allocated Player struct, or NULL on
 * failure.
 */
Player *createPlayer(float xAxis, unsigned height, unsigned width);

/**
 * @brief Frees the memory allocated for the player.
 * @param player Pointer to the Player struct to free. Safe to pass NULL.
 */
void destroyPlayer(Player *player);

/**
 * @brief Sets the player's intention to move based on input.
 * This updates the internal velocity vector but does not move the player
 * immediately.
 *
 * @param player    Pointer to the Player.
 * @param direction Direction enum (MOVE_LEFT, MOVE_RIGHT, or MOVE_NONE).
 */
void setPlayerDirection(Player *player, Direction direction);

/**
 * @brief Updates the player's physics and cooldowns.
 *
 * This function:
 * 1. Decrements the `shootTimer` by `deltaTime`.
 * 2. Updates position: `x += velocity * deltaTime`.
 * 3. Clamps the position so the player cannot move off-screen (0 to
 * screenWidth).
 *
 * @param player      Pointer to the Player.
 * @param deltaTime   Time passed since the last frame (seconds).
 * @param screenWidth The logical width of the screen (for boundary checks).
 */
void updatePlayer(Player *player, float deltaTime, unsigned screenWidth);

/**
 * @brief Internal helper to check if the weapon is ready.
 * @param player Pointer to the Player.
 * @return true if `shootTimer` <= 0.
 */
bool canPlayerShoot(Player *player);

/**
 * @brief Attempts to fire a projectile.
 *
 * Checks cooldown logic. If ready:
 * 1. Finds a free slot in the `Projectiles` pool.
 * 2. Spawns a new bullet at the player's center X.
 * 3. Resets `shootTimer` to `PLAYER_SHOOT_COOLDOWN`.
 *
 * @param player      Pointer to the Player.
 * @param projectiles Pointer to the Projectile pool manager.
 * @return true  If the shot was fired successfully.
 * @return false If the weapon is cooling down or the projectile pool is full.
 */
bool playerShoot(Player *player, Projectiles *projectiles);

#endif // PLAYER_H