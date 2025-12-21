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
}

bool canPlayerShoot(Player *player) {
  if (!player)
    return false;
  if (player->shootTimer == 0) {
    player->shootTimer = COOLDOWN;
    return true;
    ;
  }
  return false;
}
