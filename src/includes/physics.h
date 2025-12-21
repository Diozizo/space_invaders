#ifndef PHYSICS_H
#define PHYSICS_H

#include "enemy.h"
#include "explosion.h" // <--- Add this
#include "player.h"
#include "projectile.h"
#include <stdbool.h>

// Updated signature: now accepts ExplosionManager
bool checkCollisions(Player *player, Swarm *swarm, Projectiles *projectiles,
                     ExplosionManager *explosions);

#endif