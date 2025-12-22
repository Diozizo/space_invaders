#ifndef BUNKER_H
#define BUNKER_H

#include "projectile.h"
#include <stdbool.h>

#define BUNKER_COUNT 4
#define BUNKER_ROWS 8
#define BUNKER_COLS 10
#define BLOCK_SIZE 8.0f

typedef struct {
  float x, y;
  bool active;
} BunkerBlock;

typedef struct {
  float x, y;
  BunkerBlock blocks[BUNKER_ROWS * BUNKER_COLS];
} Bunker;

typedef struct {
  Bunker bunkers[BUNKER_COUNT];
} BunkerManager;

BunkerManager *createBunkers(unsigned screenWidth);
void destroyBunkers(BunkerManager *bm);

bool checkBunkerCollision(BunkerManager *bm, Projectile *p);

void resetBunkers(BunkerManager *bm, unsigned screenWidth);

#endif