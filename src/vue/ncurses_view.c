#define _POSIX_C_SOURCE 200809L
#include "../../includes/ncurses_view.h"
#include <ncurses.h>
#include <stdlib.h>

/**
 * @brief Helper: Maps Game X (0.0 -> gameWidth) to Terminal Col (0 -> COLS).
 * Handles the aspect ratio scaling.
 */
static int mapX(Ncurses_Context *ctx, float x) {
  // Formula: (EntityPosition / TotalGameWidth) * TotalTerminalColumns
  return (int)((x / (float)ctx->gameWidth) * ctx->cols);
}

/**
 * @brief Helper: Maps Game Y (0.0 -> gameHeight) to Terminal Row (0 -> ROWS).
 */
static int mapY(Ncurses_Context *ctx, float y) {
  return (int)((y / (float)ctx->gameHeight) * ctx->rows);
}

Ncurses_Context *initNcursesView(unsigned width, unsigned height) {
  Ncurses_Context *ctx = (Ncurses_Context *)malloc(sizeof(Ncurses_Context));
  if (!ctx)
    return NULL;

  // --- Ncurses Setup ---
  initscr();            // Start Ncurses mode
  cbreak();             // Line buffering disabled (pass everything to us)
  noecho();             // Don't print user keypresses to screen automatically
  keypad(stdscr, TRUE); // Enable F1, Arrow Keys, etc.
  curs_set(0);          // Hide the blinking cursor

  // CRITICAL: nodelay makes getch() non-blocking.
  // If user hasn't pressed a key, it returns ERR immediately so game loop
  // continues.
  nodelay(stdscr, TRUE);

  getmaxyx(stdscr, ctx->rows, ctx->cols); // Get actual terminal size
  ctx->gameWidth = width;
  ctx->gameHeight = height;
  ctx->win = stdscr;
  return ctx;
}

void destroyNcursesView(Ncurses_Context *ctx) {
  if (ctx) {
    endwin(); // Restore normal terminal behavior
    free(ctx);
  }
}

void renderNcurses(Ncurses_Context *ctx, const Player *p,
                   const Projectiles *prj, const Swarm *s,
                   const ExplosionManager *ex, const BunkerManager *b,
                   GameState state, bool won) {
  erase(); // Clear the screen buffer

  // --- UI: MENU SCREEN ---
  if (state == STATE_MENU) {
    mvprintw(ctx->rows / 2, ctx->cols / 2 - 10, "PRESS ENTER TO START");
    mvprintw(ctx->rows / 2 + 1, ctx->cols / 2 - 10, "PRESS Q TO QUIT");
  }

  // --- UI: GAME OVER SCREEN ---
  else if (state == STATE_GAME_OVER) {
    if (won)
      mvprintw(ctx->rows / 2, ctx->cols / 2 - 5, "YOU WIN!");
    else
      mvprintw(ctx->rows / 2, ctx->cols / 2 - 5, "GAME OVER");
    mvprintw(ctx->rows / 2 + 2, ctx->cols / 2 - 10, "PRESS ENTER");
  }

  // --- GAMEPLAY RENDER ---
  else {

    // 1. RENDER PLAYER (Procedural ASCII Art)
    if (p) {
      // Scale player width to terminal columns
      int shipWidthCols = (int)((p->width / (float)ctx->gameWidth) * ctx->cols);

      // Enforce minimum size so it doesn't disappear
      if (shipWidthCols < 3)
        shipWidthCols = 3;

      int startCol = mapX(ctx, p->x);
      int y = mapY(ctx, p->y);

      // Draw Ship Body: <=====>
      if (startCol >= 0 && startCol < ctx->cols) {
        for (int i = 0; i < shipWidthCols; i++) {
          char bodyPart = '=';
          if (i == 0)
            bodyPart = '<';
          else if (i == shipWidthCols - 1)
            bodyPart = '>';
          mvaddch(y, startCol + i, bodyPart);
        }
        // Draw Cockpit: ^
        int centerOffset = shipWidthCols / 2;
        mvaddch(y - 1, startCol + centerOffset, '^');
      }
    }

    // 2. RENDER ENEMIES & BOSS
    if (s) {
      // A. Standard Enemies (Simple 'M')
      for (int i = 0; i < TOTAL_ENEMIES; i++) {
        if (s->enemies[i].active)
          mvaddch(mapY(ctx, s->enemies[i].y), mapX(ctx, s->enemies[i].x), 'M');
      }

      // B. Boss (Complex Multi-line ASCII Art)
      if (s->level == 2 && s->boss.active) {
        // Calculate Boss Hitbox Width in Columns
        int bossWidthCols =
            (int)((s->boss.width / (float)ctx->gameWidth) * ctx->cols);
        if (bossWidthCols < 6)
          bossWidthCols = 6; // Min size for visual clarity

        int bx = mapX(ctx, s->boss.x);
        int by = mapY(ctx, s->boss.y);

        // Draw HP Bar
        mvprintw(by - 1, bx, "BOSS HP:%d", s->boss.health);

        // Draw Mothership Sprite Logic:
        // Top:    /--------\    
        // Middle: | O  O  O |
        // Bottom: \--------/
        for (int i = 0; i < bossWidthCols; i++) {
          char top = '-', mid = ' ', bot = '-';

          if (i == 0) {
            top = '/';
            mid = '|';
            bot = '\\'; // Left Edge
          } else if (i == bossWidthCols - 1) {
            top = '\\';
            mid = '|';
            bot = '/'; // Right Edge
          } else {
            if (i % 2 != 0)
              mid = '0'; // Lights/Windows in middle
          }

          mvaddch(by, bx + i, top);
          mvaddch(by + 1, bx + i, mid);
          mvaddch(by + 2, bx + i, bot);
        }
      }
    }

    // 3. RENDER PROJECTILES
    if (prj) {
      for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (prj->projectiles[i].active)
          mvaddch(mapY(ctx, prj->projectiles[i].y),
                  mapX(ctx, prj->projectiles[i].x), '|');
      }
    }

    // 4. RENDER BUNKERS
    if (b) {
      for (int k = 0; k < BUNKER_COUNT; k++) {
        for (int i = 0; i < BUNKER_ROWS * BUNKER_COLS; i++) {
          // Draw individual active blocks
          if (b->bunkers[k].blocks[i].active)
            mvaddch(mapY(ctx, b->bunkers[k].blocks[i].y),
                    mapX(ctx, b->bunkers[k].blocks[i].x), '#');
        }
      }
    }

    // 5. RENDER EXPLOSIONS
    if (ex) {
      for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (ex->explosions[i].active) {
          mvaddch(mapY(ctx, ex->explosions[i].y),
                  mapX(ctx, ex->explosions[i].x), '*');
        }
      }
    }
  }
  refresh(); // Push buffer to screen
}