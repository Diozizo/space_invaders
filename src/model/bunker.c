#include "../includes/bunker.h"
#include <stdio.h>
#include <stdlib.h>

bool checkBlockOverlap(float bx, float by, float bw, float bh, float ox,
                       float oy, float ow, float oh) {
  return (bx < ox + ow && bx + bw > ox && by < oy + oh && by + bh > oy);
}

void initBunkerShape(Bunker *b, float startX, float startY) {
  if (!b)
    return;

  b->x = startX;
  b->y = startY;

  int i = 0;
  for (int row = 0; row < BUNKER_ROWS; row++) {
    for (int col = 0; col < BUNKER_COLS; col++) {
      b->blocks[i].x = startX + col * BLOCK_SIZE;
      b->blocks[i].y = startY + row * BLOCK_SIZE;
      b->blocks[i].active = true;

      if (row >= 5 && (col >= 3 && col <= 6)) {
        b->blocks[i].active = false;
      }

      if (row == 0 && (col == 0 || col == BUNKER_COLS - 1)) {
        b->blocks[i].active = false;
      }

      i++;
    }
  }
}

BunkerManager *createBunkers(unsigned screenWidth) {
  BunkerManager *bm = (BunkerManager *)calloc(1, sizeof(BunkerManager));
  if (!bm) {
    return NULL;
  }

  resetBunkers(bm, screenWidth);
  return bm;
}

void resetBunkers(BunkerManager *bm, unsigned screenWidth) {
  if (!bm)
    return;

  float bunkerWidth = BUNKER_COLS * BLOCK_SIZE;
  float totalWidth = BUNKER_COUNT * bunkerWidth;

  float gap = (screenWidth - totalWidth) / (BUNKER_COUNT + 1);
  float yPos = 450.0f;

  for (int i = 0; i < BUNKER_COUNT; i++) {
    float xPos = gap + (i * (bunkerWidth + gap));
    initBunkerShape(&bm->bunkers[i], xPos, yPos);
  }
}

void destroyBunkers(BunkerManager *bm) {
  if (bm)
    free(bm);
}

bool checkBunkerCollision(BunkerManager *bm, Projectile *p) {
  if (!bm || !p || !p->active)
    return false;

  for (int i = 0; i < BUNKER_COUNT; i++) {
    Bunker *b = &bm->bunkers[i];

    for (int j = 0; j < BUNKER_ROWS * BUNKER_COLS; j++) {
      BunkerBlock *block = &b->blocks[j];

      if (block->active &&
          checkBlockOverlap(block->x, block->y, BLOCK_SIZE, BLOCK_SIZE, p->x,
                            p->y, p->w, p->h)) {
        // Collision detected
        block->active = false;
        p->active = false;
        return true;
      }
    }
  }
  return false;
}