#include "../../includes/bunker.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Helper function for AABB (Axis-Aligned Bounding Box) collision.
 * Checks if two rectangles overlap.
 */
bool checkBlockOverlap(float bx, float by, float bw, float bh, float ox,
                       float oy, float ow, float oh) {
  return (bx < ox + ow && bx + bw > ox && by < oy + oh && by + bh > oy);
}

/**
 * @brief Initializes the grid of blocks for a single bunker.
 * "Sculpts" the classic shape by deactivating specific blocks in the grid.
 */
void initBunkerShape(Bunker *b, float startX, float startY) {
  if (!b)
    return;

  b->x = startX;
  b->y = startY;

  int i = 0;
  for (int row = 0; row < BUNKER_ROWS; row++) {
    for (int col = 0; col < BUNKER_COLS; col++) {
      // Position each block relative to the bunker's start (Top-Left)
      b->blocks[i].x = startX + col * BLOCK_SIZE;
      b->blocks[i].y = startY + row * BLOCK_SIZE;
      b->blocks[i].active = true;

      // --- Shape Sculpting Logic ---

      // Cut out the bottom arch (middle columns of the bottom rows)
      if (row >= 5 && (col >= 3 && col <= 6)) {
        b->blocks[i].active = false;
      }

      // Cut out the top corners (rounding the top edge)
      if (row == 0 && (col == 0 || col == BUNKER_COLS - 1)) {
        b->blocks[i].active = false;
      }

      i++;
    }
  }
}

BunkerManager *createBunkers(unsigned screenWidth) {
  // Use calloc to ensure all memory is zeroed out initially
  BunkerManager *bm = (BunkerManager *)calloc(1, sizeof(BunkerManager));
  if (!bm) {
    return NULL;
  }

  // Set initial positions and states
  resetBunkers(bm, screenWidth);
  return bm;
}

void resetBunkers(BunkerManager *bm, unsigned screenWidth) {
  if (!bm)
    return;

  // Calculate layout to center bunkers evenly
  float bunkerWidth = BUNKER_COLS * BLOCK_SIZE;
  float totalWidth = BUNKER_COUNT * bunkerWidth;

  // Distribute the empty space equally between bunkers and screen edges
  float gap = (screenWidth - totalWidth) / (BUNKER_COUNT + 1);
  float yPos = 450.0f; // Fixed vertical height for shields

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

  // Check every bunker
  for (int i = 0; i < BUNKER_COUNT; i++) {
    Bunker *b = &bm->bunkers[i];

    // Check every block in the bunker
    for (int j = 0; j < BUNKER_ROWS * BUNKER_COLS; j++) {
      BunkerBlock *block = &b->blocks[j];

      // Only check collision if the block is still part of the shield
      if (block->active &&
          checkBlockOverlap(block->x, block->y, BLOCK_SIZE, BLOCK_SIZE, p->x,
                            p->y, p->w, p->h)) {

        // Destroy the block (erode the shield)
        block->active = false;

        // Destroy the projectile
        p->active = false;
        return true;
      }
    }
  }
  return false;
}