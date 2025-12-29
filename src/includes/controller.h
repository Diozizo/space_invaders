#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "game_state.h"
#include "player.h"
#include "projectile.h"
#include "sdl_view.h"
#include <stdbool.h>

/**
 * @file controller.h
 * @brief Interface for handling user input in the SDL version of the game.
 * * This module acts as the "Controller" in the MVC architecture. It polls
 * the SDL event queue for keyboard, mouse, and gamepad events, translating
 * them into actions for the Model (Player/Projectiles) or the View
 * (Window/Audio).
 */

// ==========================================
//               FUNCTIONS
// ==========================================

/**
 * @brief Processes all pending input events for the frame.
 * * This function polls the SDL Event Loop. It handles:
 * - **System Events:** Window closing (clicking X).
 * - **Game Logic:** Player movement (Left/Right) and Shooting (Space/Button).
 * - **State Management:** Toggling between Menu, Playing, Paused, and Game
 * Over.
 * - **View Actions:** Toggling fullscreen mode.
 * * It supports both Keyboard and Gamepad (Controller) input.
 * * @param player      Pointer to the player object to update based on input.
 * @param projectiles Pointer to the projectile pool (to spawn bullets on
 * shoot).
 * @param view        Pointer to the SDL context (for audio playback and window
 * management).
 * @param gameState   Pointer to the current game state (allows transitions like
 * Pause/Resume).
 * * @return true  If the game loop should continue running.
 * @return false If the user requested to Quit the application (ESC or Close
 * Window).
 */
bool handleInput(Player *player, Projectiles *projectiles, SDL_Context *view,
                 GameState *gameState);

#endif // CONTROLLER_H