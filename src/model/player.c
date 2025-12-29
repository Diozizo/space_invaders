#include "../includes/player.h"
#include <stdbool.h>
#include <stdlib.h>

Player *createPlayer(float xAxis, unsigned height, unsigned width) {
  // Use calloc to ensure all fields (score, timers) start at 0
  Player *p = (Player *)calloc(1, sizeof(Player));
  if (!p) {
    return NULL;
  }

  // Set initial game state
  p->health = HEALTH;
  p->shootTimer = 0.0f;

  // Set physics state
  p->velocityX = 0.0f;
  p->width = width;
  p->height = height;
  p->x = xAxis;
  p->y = Y_AXIS; // Fixed vertical position

  p->score = 0;

  // Set animation state
  p->animTimer = 0.0f;
  p->animFrame = 0;
  p->animDir = 1; // Start animating forward

  return p;
}

void destroyPlayer(Player *player) {
  if (!player) {
    return;
  }
  free(player);
}

void setPlayerDirection(Player *player, Direction direction) {
  if (!player) {
    return;
  }

  // Map input Enum to actual Velocity float
  if (direction == MOVE_NONE) {
    player->velocityX = 0.0f;
  } else if (direction == MOVE_LEFT) {
    player->velocityX = -PLAYER_SPEED;
  } else {
    player->velocityX = PLAYER_SPEED;
  }
}

void updatePlayer(Player *player, float deltaTime, unsigned screenWidth) {
  if (!player) {
    return;
  }

  // --- 1. Physics Update ---
  player->x += player->velocityX * deltaTime;

  // Clamp Position: Keep player inside screen bounds (0 to Width)
  if (player->x < 0) {
    player->x = 0;
  }
  if (player->x > screenWidth - player->width) {
    player->x = screenWidth - player->width;
  }

  // --- 2. Cooldown Update ---
  if (player->shootTimer > 0) {
    player->shootTimer -= deltaTime;
    if (player->shootTimer < 0)
      player->shootTimer = 0;
  }

  // --- 3. Animation Update (Idle Exhaust) ---
  player->animTimer += deltaTime;
  if (player->animTimer >= 0.3f) { // Toggle frame every 0.3 seconds
    player->animTimer = 0.0f;
    player->animFrame += player->animDir;

    // Ping-Pong Animation Logic: 0 -> 1 -> 2 -> 3 -> 2 -> 1 -> 0
    if (player->animFrame >= 3) {
      player->animFrame = 3;
      player->animDir = -1; // Reverse direction
    } else if (player->animFrame <= 0) {
      player->animFrame = 0;
      player->animDir = 1; // Forward direction
    }
  }
}

bool canPlayerShoot(Player *player) {
  if (!player)
    return false;

  if (player->shootTimer == 0) {
    player->shootTimer = PLAYER_SHOOT_COOLDOWN;
    return true;
  }
  return false;
}

bool playerShoot(Player *player, Projectiles *projectiles) {
  if (!player || !projectiles)
    return false;

  // 1. Check Cooldown
  if (player->shootTimer <= 0.0f) {

    // 2. Calculate Position (Center the bullet on the player sprite)
    // Formula: PlayerX + (Half Player Width) - (Half Bullet Width)
    float bulletX =
        player->x + (player->width / 2.0f) - (PROJECTILE_WIDTH / 2.0f);

    float bulletY = player->y; // Spawn at top of player

    // 3. Fire! (Spawn projectile moving UP)
    spawnProjectile(projectiles, bulletX, bulletY, MOVE_UP);

    // 4. Reset Cooldown
    player->shootTimer = PLAYER_SHOOT_COOLDOWN;

    return true;
  }

  return false;
}