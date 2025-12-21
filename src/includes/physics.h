#ifndef PHYSICS_H
#define PHYSICS_H

#include "enemy.h"
#include "player.h"
#include "projectile.h"
#include <stdbool.h>

/**
 * @brief Checks for overlaps between bullets and entities.
 * * 1. Player Bullets (UP) vs Enemies -> Kills Enemy
 * 2. Enemy Bullets (DOWN) vs Player -> Hurts Player
 * * @return true if the player was destroyed (Health <= 0)
 */
bool checkCollisions(Player *player, Swarm *swarm, Projectiles *projectiles);

#endif