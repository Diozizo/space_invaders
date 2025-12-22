#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "player.h"
#include "projectile.h"
#include "sdl_view.h"
#include <stdbool.h>

/**
 * @brief Processes all input events (Keyboard, Window events).
 * @param player Pointer to the player object to control.
 * @param projectiles Pointer to the projectile pool (for shooting).
 * @return false if the user requested to Quit.
 */
bool handleInput(Player *player, Projectiles *projectiles, SDL_Context *view);

#endif