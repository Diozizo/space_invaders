#include "../includes/player.h"
#include <stdbool.h>
#include <stdlib.h>

Player *createPlayer(float xAxis, unsigned height, unsigned width) {
  Player *p = (Player *)malloc(sizeof(Player));
  if (!p) {
    // handle error will do that later
    return NULL;
  }
  p->health = HEALTH;
  p->shootTimer = 0.0f;
  p->velocityX = 0.0f;
  p->width = width;
  p->height = height;
  p->x = xAxis;
  p->y = Y_AXIS;
  p->score = 0;
  p->animTimer = 0.0f;
  p->animFrame = 0;
  p->animDir = 1;

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
  player->x += player->velocityX * deltaTime;
  if (player->x < 0) {
    player->x = 0;
  }
  if (player->x > screenWidth - player->width) {
    player->x = screenWidth - player->width;
  }

  if (player->shootTimer > 0) {
    player->shootTimer -= deltaTime;
    if (player->shootTimer < 0)
      player->shootTimer = 0;
  }

  player->animTimer += deltaTime;
  if (player->animTimer >= 0.3f) {
    player->animTimer = 0.0f;
    player->animFrame += player->animDir;
    if (player->animFrame >= 3) {
      player->animFrame = 3;
      player->animDir = -1;
    } else if (player->animFrame <= 0) {
      player->animFrame = 0;
      player->animDir = 1;
    }
  }
}

bool canPlayerShoot(Player *player) {
  if (!player)
    return false;
  if (player->shootTimer == 0) {
    player->shootTimer = COOLDOWN;
    return true;
  }
  return false;
}

bool playerShoot(Player *player, Projectiles *projectiles) {
  if (!player || !projectiles)
    return false;

  // 1. Check Cooldown
  if (player->shootTimer <= 0.0f) {

    // 2. Calculate Position (Center of Player)
    float bulletX =
        player->x + (player->width / 2.0f) - (PROJECTILE_WIDTH / 2.0f);
    float bulletY = player->y; // Top of player

    // 3. Fire!
    spawnProjectile(projectiles, bulletX, bulletY, MOVE_UP);

    // 4. Reset Cooldown
    player->shootTimer = PLAYER_SHOOT_COOLDOWN;

    return true;
  }

  return false;
}