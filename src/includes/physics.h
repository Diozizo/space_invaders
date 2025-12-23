#ifndef PHYSICS_H
#define PHYSICS_H

#include "bunker.h"
#include "enemy.h"
#include "explosion.h"
#include "player.h"
#include "projectile.h"
#include <stdbool.h>

// Updated signature: now accepts ExplosionManager and BunkerManager
bool checkCollisions(Player *player, Swarm *swarm, Projectiles *projectiles,
                     ExplosionManager *explosions, BunkerManager *bunkers,
                     bool *enemyHit);
#endif