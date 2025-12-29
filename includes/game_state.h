#ifndef GAME_STATE_H
#define GAME_STATE_H

/**
 * @file game_state.h
 * @brief Defines the high-level states of the application.
 * * This enum is the core of the state machine that controls the game loop.
 * The View and Controller modules use this to decide what to render (Menu vs
 * Game) and how to interpret input (Enter to Start vs Enter to Pause).
 */

/**
 * @brief Enumeration of all possible game states.
 */
typedef enum {
  /** * @brief The Main Menu screen.
   * Displays the title and instructions. Waiting for the user to start.
   */
  STATE_MENU,

  /** * @brief The active gameplay loop.
   * Player can move and shoot, enemies move, physics are updated.
   */
  STATE_PLAYING,

  /** * @brief The Pause screen.
   * Gameplay is frozen (delta time ignored), but the game is still rendered
   * with an overlay. Player input is restricted to unpausing or quitting.
   */
  STATE_PAUSED,

  /** * @brief The Game Over screen.
   * Triggered when the player loses all lives or clears all levels.
   * Displays the final score and outcome (Win/Loss). waiting for restart.
   */
  STATE_GAME_OVER
} GameState;

#endif // GAME_STATE_H