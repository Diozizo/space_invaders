#ifndef NCURSES_VIEW_H
#define NCURSES_VIEW_H

#include "bunker.h"
#include "enemy.h"
#include "explosion.h"
#include "game_state.h"
#include "player.h"
#include "projectile.h"
#include <ncurses.h>

/**
 * @file ncurses_view.h
 * @brief Handles the text-based rendering of the game (The "View" in MVC).
 * * This module manages the Ncurses library lifecycle. It is responsible for:
 * - Setting up the terminal (raw mode, no echo, invisible cursor).
 * - Translating game entity float coordinates into integer terminal rows/cols.
 * - Drawing ASCII characters to represent game objects (e.g., '^' for player,
 * 'M' for aliens).
 * - Handling screen refreshes and window management.
 */

// ==========================================
//               STRUCTURES
// ==========================================

/**
 * @brief Context structure holding Ncurses specific state.
 * Wraps the Ncurses window and dimension data to pass around the application.
 */
typedef struct {
  int rows; /**< Actual height of the terminal window in characters. */
  int cols; /**< Actual width of the terminal window in characters. */
  unsigned gameWidth; /**< Logical game width (used for coordinate scaling). */
  unsigned
      gameHeight; /**< Logical game height (used for coordinate scaling). */
  WINDOW *win;    /**< Pointer to the Ncurses window structure. */
} Ncurses_Context;

// ==========================================
//               FUNCTIONS
// ==========================================

/**
 * @brief Initializes the Ncurses environment.
 * * Sets up standard Ncurses modes:
 * - `initscr()`: Starts curses mode.
 * - `cbreak()`: Line buffering disabled.
 * - `noecho()`: Don't print user input keypresses.
 * - `curs_set(0)`: Hide the blinking cursor.
 * - `keypad()`: Enable F1, Arrow keys, etc.
 * * @param width  The logical width of the game.
 * @param height The logical height of the game.
 * @return Ncurses_Context* Pointer to the created context, or NULL on failure.
 */
Ncurses_Context *initNcursesView(unsigned width, unsigned height);

/**
 * @brief Restores the terminal settings and cleans up memory.
 * Calls `endwin()` to exit Ncurses mode and restore normal terminal behavior.
 * @param ctx Pointer to the context to free.
 */
void destroyNcursesView(Ncurses_Context *ctx);

/**
 * @brief Main render function. Draws the entire game state to the terminal.
 * * This function clears the previous frame and redraws all entities.
 * It handles the scaling logic to map the high-resolution game coordinates
 * (e.g., 0-800) down to the low-resolution terminal grid (e.g., 0-80).
 * * @param ctx   Pointer to the Ncurses context.
 * @param p     Pointer to the Player entity.
 * @param prj   Pointer to the Projectile manager.
 * @param s     Pointer to the Swarm (enemies).
 * @param ex    Pointer to the Explosion manager.
 * @param b     Pointer to the Bunker manager.
 * @param state Current GameState (determines if we draw menu/game/gameover).
 * @param won   Boolean flag indicating if the player won (for Game Over text).
 */
void renderNcurses(Ncurses_Context *ctx, const Player *p,
                   const Projectiles *prj, const Swarm *s,
                   const ExplosionManager *ex, const BunkerManager *b,
                   GameState state, bool won);

#endif // NCURSES_VIEW_H