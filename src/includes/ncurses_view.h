#ifndef NCURSES_VIEW_H
#define NCURSES_VIEW_H

#include "bunker.h"
#include "enemy.h"
#include "explosion.h"
#include "game_state.h"
#include "player.h"
#include "projectile.h"
#include <ncurses.h>

typedef struct {
  int rows;
  int cols;
  unsigned gameWidth;
  unsigned gameHeight;
  WINDOW *win;
} Ncurses_Context;

Ncurses_Context *initNcursesView(unsigned width, unsigned height);
void destroyNcursesView(Ncurses_Context *ctx);
void renderNcurses(Ncurses_Context *ctx, const Player *p,
                   const Projectiles *prj, const Swarm *s,
                   const ExplosionManager *ex, const BunkerManager *b,
                   GameState state, bool won);

#endif