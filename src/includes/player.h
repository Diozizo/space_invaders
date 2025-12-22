#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

/**
 * @brief The movement speed of the player in pixels per second.
 * Used for calculating smooth movement based on Delta Time.
 */
#define PLAYER_SPEED 300.0f

/**
 * @brief The fixed Y position of the player (vertical lock).
 * Note: Check if 200 is appropriate for your 600px height screen.
 * Usually, Space Invaders are near the bottom (e.g., 500 or 550).
 */
#define Y_AXIS 200.0f

/**
 * @brief the health of the player at the start of the level
 */
#define HEALTH 3

/**
 * @brief Time in seconds required between two shots.
 */
#define COOLDOWN 0.2f

/**
 * @brief Enum representing the input state for horizontal movement.
 */
typedef enum {
  MOVE_NONE = 0,  /**< No movement input */
  MOVE_LEFT = -1, /**< Moving towards 0 (Left) */
  MOVE_RIGHT = 1  /**< Moving towards Screen Width (Right) */
} Direction;

/**
 * @brief Structure representing the Player entity.
 */
typedef struct _player {
  float x;         /**< Horizontal position (float for smooth physics) */
  float y;         /**< Vertical position (fixed in Space Invaders) */
  unsigned height; /**< Sprite height in pixels */
  unsigned width;  /**< Sprite width in pixels */
  float velocityX; /**< Current horizontal speed (pixels/sec) */
  float
      shootTimer;  /**< Countdown timer for reloading. 0 means ready to fire. */
  unsigned health; /**< Current lives/health remaining */
  unsigned score;
  float animTimer;
  int animFrame;
  int animDir;
} Player;

/**
 * @brief Allocates memory for a new Player and initializes its values.
 * * @param xAxis  The starting X coordinate.
 * @param height The height of the player sprite.
 * @param width  The width of the player sprite.
 * @return Player* Pointer to the newly allocated Player struct, or NULL if
 * allocation fails.
 */
Player *createPlayer(float xAxis, unsigned height, unsigned width);

/**
 * @brief Frees the memory allocated for the player.
 * * @param player Pointer to the Player struct to free. Safe to pass NULL.
 */
void destroyPlayer(Player *player);

/**
 * @brief Sets the player's intention to move.
 * * @param player    Pointer to the Player.
 * @param direction Direction enum (MOVE_LEFT, MOVE_RIGHT, or MOVE_NONE).
 */
void setPlayerDirection(Player *player, Direction direction);

/**
 * @brief Updates the player's physics and state.
 * Handles movement calculation and cooldown timer reduction.
 * * @param player      Pointer to the Player.
 * @param deltaTime   Time passed since the last frame (in seconds).
 * @param screenWidth The logical width of the screen (for boundary checks).
 */
void updatePlayer(Player *player, float deltaTime, unsigned screenWidth);

/**
 * @brief Checks if the player is allowed to shoot.
 * If the timer is 0, this resets the timer to COOLDOWN and returns true.
 * * @param player Pointer to the Player.
 * @return true  If the shot was fired successfully.
 * @return false If the weapon is still cooling down.
 */
bool canPlayerShoot(Player *player);

#endif // PLAYER_H
