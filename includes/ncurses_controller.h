#ifndef NCURSES_CONTROLLER_H
#define NCURSES_CONTROLLER_H

#include "game_state.h"
#include "player.h"
#include "projectile.h"
#include <stdbool.h>

/**
 * @file ncurses_controller.h
 * @brief Interface for handling user input in the Ncurses (Terminal) mode.
 *
 * This module adapts the game control logic for a text-based environment.
 * Unlike SDL, Ncurses input is character-stream based and often requires
 * specific timeout handling (`nodelay` or `timeout`) which this controller
 * manages.
 */

// ==========================================
//               FUNCTIONS
// ==========================================

/**
 * @brief Processes keyboard input for the terminal interface.
 *
 * Reads characters from the standard input (`getch()`). Supports:
 * - **Arrow Keys / 'a', 'd':** Player movement.
 * - **Space:** Shooting.
 * - **'p':** Pause/Unpause.
 * - **Enter:** Start Game / Restart.
 * - **'q':** Quit.
 *
 * @param player     Pointer to the player object to control.
 * @param bullets    Pointer to the projectile pool (to spawn bullets).
 * @param state      Pointer to the current game state (Menu/Playing/etc.).
 * @param needsReset Pointer to a boolean flag. The controller sets this to true
 * when the user requests a game restart (e.g., pressing Enter
 * at the Game Over screen), signaling the main loop to reset entities.
 * @param deltaTime  Time elapsed since last frame (used for input buffering or
 * cooldowns).
 *
 * @return true  If the game loop should continue.
 * @return false If the user requested to Quit ('q').
 */
bool handleNcursesInput(Player *player, Projectiles *bullets, GameState *state,
                        bool *needsReset, float deltaTime);

#endif // NCURSES_CONTROLLER_H