#ifndef PHYSICS_H
#define PHYSICS_H

#include "bunker.h"
#include "enemy.h"
#include "explosion.h"
#include "player.h"
#include "projectile.h"
#include <stdbool.h>

/**
 * @file physics.h
 * @brief The core Physics Engine for collision detection.
 *
 * This module acts as the "Referee" of the game. It checks for overlaps
 * between the various game entities (Axis-Aligned Bounding Box checks).
 * It modifies the state of entities directly (e.g., setting `active = false`)
 * and triggers events like explosions or game over states.
 */

// ==========================================
//               FUNCTIONS
// ==========================================

/**
 * @brief Performs all collision checks for a single frame.
 *
 * This function iterates through all active entities and checks:
 * 1. **Projectiles vs Enemies:** If a player bullet hits an alien, both are
 * destroyed, an explosion is spawned, and the score is updated.
 * 2. **Projectiles vs Player:** If an enemy bullet hits the player, damage is
 * taken.
 * 3. **Projectiles vs Bunkers:** If any bullet hits a bunker block, the block
 * erodes.
 * 4. **Swarm vs Player:** (Optional) If the aliens physically touch the player.
 * 5. **Swarm vs Bunkers:** If aliens descend into the shields, the shields are
 * destroyed.
 *
 * @param player      Pointer to the Player entity (to check if hit).
 * @param swarm       Pointer to the Swarm (to check if individual enemies are
 * hit).
 * @param projectiles Pointer to the Projectile pool (checks all active
 * bullets).
 * @param explosions  Pointer to the Explosion Manager (to spawn effects on
 * hit).
 * @param bunkers     Pointer to the Bunker Manager (to check shield damage).
 * @param enemyHit    [Output] Pointer to a bool flag. Set to `true` if *any*
 * enemy was destroyed this frame (useful for playing sound effects).
 *
 * @return true  If the Player was hit and died (Game Over condition).
 * @return false If the Player survived the frame.
 */
bool checkCollisions(Player *player, Swarm *swarm, Projectiles *projectiles,
                     ExplosionManager *explosions, BunkerManager *bunkers,
                     bool *enemyHit);

#endif // PHYSICS_H