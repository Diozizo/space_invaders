#define _POSIX_C_SOURCE 200809L
#include "../includes/ncurses_view.h"
#include <ncurses.h>
#include <stdlib.h>

// Helper to map coordinates
static int mapX(Ncurses_Context *ctx, float x) {
  return (int)((x / (float)ctx->gameWidth) * ctx->cols);
}
static int mapY(Ncurses_Context *ctx, float y) {
  return (int)((y / (float)ctx->gameHeight) * ctx->rows);
}

Ncurses_Context *initNcursesView(unsigned width, unsigned height) {
  Ncurses_Context *ctx = (Ncurses_Context *)malloc(sizeof(Ncurses_Context));
  if (!ctx)
    return NULL;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  nodelay(stdscr, TRUE); // Important!

  getmaxyx(stdscr, ctx->rows, ctx->cols);
  ctx->gameWidth = width;
  ctx->gameHeight = height;
  ctx->win = stdscr;
  return ctx;
}

void destroyNcursesView(Ncurses_Context *ctx) {
  if (ctx) {
    endwin();
    free(ctx);
  }
}

void renderNcurses(Ncurses_Context *ctx, const Player *p,
                   const Projectiles *prj, const Swarm *s,
                   const ExplosionManager *ex, const BunkerManager *b,
                   GameState state, bool won) {
  erase();

  if (state == STATE_MENU) {
    mvprintw(ctx->rows / 2, ctx->cols / 2 - 10, "PRESS ENTER TO START");
    mvprintw(ctx->rows / 2 + 1, ctx->cols / 2 - 10, "PRESS Q TO QUIT");
  } else if (state == STATE_GAME_OVER) {
    if (won)
      mvprintw(ctx->rows / 2, ctx->cols / 2 - 5, "YOU WIN!");
    else
      mvprintw(ctx->rows / 2, ctx->cols / 2 - 5, "GAME OVER");
    mvprintw(ctx->rows / 2 + 2, ctx->cols / 2 - 10, "PRESS ENTER");
  } else {

    // 1. PLAYER (Scaled to Hitbox)
    if (p) {
      int shipWidthCols = (int)((p->width / (float)ctx->gameWidth) * ctx->cols);
      if (shipWidthCols < 3)
        shipWidthCols = 3;

      int startCol = mapX(ctx, p->x);
      int y = mapY(ctx, p->y);

      if (startCol >= 0 && startCol < ctx->cols) {
        for (int i = 0; i < shipWidthCols; i++) {
          char bodyPart = '=';
          if (i == 0)
            bodyPart = '<';
          else if (i == shipWidthCols - 1)
            bodyPart = '>';
          mvaddch(y, startCol + i, bodyPart);
        }
        int centerOffset = shipWidthCols / 2;
        mvaddch(y - 1, startCol + centerOffset, '^');
      }
    }

    // 2. ENEMIES & BOSS
    if (s) {
      // Normal Enemies
      for (int i = 0; i < TOTAL_ENEMIES; i++) {
        if (s->enemies[i].active)
          mvaddch(mapY(ctx, s->enemies[i].y), mapX(ctx, s->enemies[i].x), 'M');
      }

      // --- FIX: RENDER BOSS (Scaled to Hitbox) ---
      if (s->level == 2 && s->boss.active) {
        // Calculate Boss Hitbox Width in Columns
        int bossWidthCols =
            (int)((s->boss.width / (float)ctx->gameWidth) * ctx->cols);
        if (bossWidthCols < 6)
          bossWidthCols = 6; // Min size so sprite looks okay

        int bx = mapX(ctx, s->boss.x);
        int by = mapY(ctx, s->boss.y);

        // Draw HP Bar above boss
        mvprintw(by - 1, bx, "BOSS HP:%d", s->boss.health);

        // Draw Mothership Sprite
        // Top:    /--------\
          // Middle: | O  O  O |
        // Bottom: \--------/
        for (int i = 0; i < bossWidthCols; i++) {
          char top = '-', mid = ' ', bot = '-';

          if (i == 0) {
            top = '/';
            mid = '|';
            bot = '\\';
          } else if (i == bossWidthCols - 1) {
            top = '\\';
            mid = '|';
            bot = '/';
          } else {
            // Decorative lights in the middle
            if (i % 2 != 0)
              mid = '0';
          }

          mvaddch(by, bx + i, top);
          mvaddch(by + 1, bx + i, mid);
          mvaddch(by + 2, bx + i, bot);
        }
      }
    }

    // 3. PROJECTILES
    if (prj) {
      for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (prj->projectiles[i].active)
          mvaddch(mapY(ctx, prj->projectiles[i].y),
                  mapX(ctx, prj->projectiles[i].x), '|');
      }
    }

    // 4. BUNKERS
    if (b) {
      for (int k = 0; k < BUNKER_COUNT; k++) {
        for (int i = 0; i < BUNKER_ROWS * BUNKER_COLS; i++) {
          if (b->bunkers[k].blocks[i].active)
            mvaddch(mapY(ctx, b->bunkers[k].blocks[i].y),
                    mapX(ctx, b->bunkers[k].blocks[i].x), '#');
        }
      }
    }

    // 5. EXPLOSIONS
    if (ex) {
      for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (ex->explosions[i].active) {
          mvaddch(mapY(ctx, ex->explosions[i].y),
                  mapX(ctx, ex->explosions[i].x), '*');
        }
      }
    }
  }
  refresh();
}