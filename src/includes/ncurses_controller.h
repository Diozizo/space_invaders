#ifndef NCURSES_CONTROLLER_H
#define NCURSES_CONTROLLER_H

#include "game_state.h"
#include "player.h"
#include "projectile.h"
#include <stdbool.h>

bool handleNcursesInput(Player *player, Projectiles *bullets, GameState *state,
                        bool *needsReset, float deltaTime);

#endif