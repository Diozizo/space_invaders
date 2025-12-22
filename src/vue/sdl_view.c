#include "../includes/sdl_view.h"
#include "../includes/explosion.h"
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

#define LOGICAL_WIDTH 800
#define LOGICAL_HEIGHT 600

// Helper to reduce repetitive code
SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *path) {
  SDL_Texture *tex = IMG_LoadTexture(renderer, path);
  if (!tex) {
    printf("Failed to load texture '%s': %s\n", path, SDL_GetError());
  }
  return tex;
}

SDL_Context *initSDLView(unsigned windowWidth, unsigned windowHeight) {
  // 1. Initialize Graphics
  SDL_SetHint("SDL_RENDER_SCALE_QUALITY", "nearest");

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    printf("SDL Init Error: %s\n", SDL_GetError());
    return NULL;
  }

  if (!TTF_Init()) {
    printf("TTF Init Error: %s\n", SDL_GetError());
    SDL_Quit();
    return NULL;
  }

  SDL_Context *ctx = (SDL_Context *)malloc(sizeof(SDL_Context));
  if (!ctx)
    return NULL;

  // 2. Create Window
  ctx->window = SDL_CreateWindow("Space Invaders", windowWidth, windowHeight,
                                 SDL_WINDOW_RESIZABLE);
  if (!ctx->window) {
    free(ctx);
    return NULL;
  }

  // 3. Create Renderer
  ctx->renderer = SDL_CreateRenderer(ctx->window, NULL);
  if (!ctx->renderer) {
    SDL_DestroyWindow(ctx->window);
    free(ctx);
    return NULL;
  }

  ctx->font = TTF_OpenFont("src/assets/font.ttf", 24);
  if (!ctx->font) {
    printf("WARNING: Failed to load font: %s\n", SDL_GetError());
  }

  // 4. Setup Logical Scaling
  if (SDL_SetRenderLogicalPresentation(ctx->renderer, LOGICAL_WIDTH,
                                       LOGICAL_HEIGHT,
                                       SDL_LOGICAL_PRESENTATION_STRETCH) < 0) {
    printf("Warning: Logical Presentation failed: %s\n", SDL_GetError());
  }
  ctx->screenWidth = LOGICAL_WIDTH;
  ctx->screenHeight = LOGICAL_HEIGHT;

  // 5. LOAD ASSETS
  ctx->backgroundTexture =
      loadTexture(ctx->renderer, "src/assets/background.png");
  ctx->playerTexture = loadTexture(ctx->renderer, "src/assets/player.png");
  ctx->enemyTexture1 = loadTexture(ctx->renderer, "src/assets/alien_1.png");
  ctx->enemyTexture2 = loadTexture(ctx->renderer, "src/assets/alien_2.png");
  ctx->playerProjectileTexture =
      loadTexture(ctx->renderer, "src/assets/bullet_1.png");
  ctx->enemyProjectileTexture =
      loadTexture(ctx->renderer, "src/assets/bullet_2.png");

  ctx->exhaustTexture[0] =
      loadTexture(ctx->renderer, "src/assets/player_fire_1.png");
  ctx->exhaustTexture[1] =
      loadTexture(ctx->renderer, "src/assets/player_fire_2.png");
  ctx->exhaustTexture[2] =
      loadTexture(ctx->renderer, "src/assets/player_fire_3.png");
  ctx->exhaustTexture[3] =
      loadTexture(ctx->renderer, "src/assets/player_fire_4.png");

  ctx->explosionTextures[0] =
      loadTexture(ctx->renderer, "src/assets/explosion_1.png");
  ctx->explosionTextures[1] =
      loadTexture(ctx->renderer, "src/assets/explosion_2.png");
  ctx->explosionTextures[2] =
      loadTexture(ctx->renderer, "src/assets/explosion_3.png");

  // Check critical assets
  if (!ctx->playerTexture || !ctx->enemyTexture1) {
    printf("CRITICAL: Failed to load game assets. Check file paths!\n");
  }

  return ctx;
}

void destroySDLView(SDL_Context *ctx) {
  if (!ctx)
    return;

  if (ctx->backgroundTexture)
    SDL_DestroyTexture(ctx->backgroundTexture);
  if (ctx->playerTexture)
    SDL_DestroyTexture(ctx->playerTexture);
  if (ctx->enemyTexture1)
    SDL_DestroyTexture(ctx->enemyTexture1);
  if (ctx->enemyTexture2)
    SDL_DestroyTexture(ctx->enemyTexture2);
  if (ctx->playerProjectileTexture)
    SDL_DestroyTexture(ctx->playerProjectileTexture);
  if (ctx->enemyProjectileTexture)
    SDL_DestroyTexture(ctx->enemyProjectileTexture);

  for (int i = 0; i < 3; i++) {
    if (ctx->explosionTextures[i])
      SDL_DestroyTexture(ctx->explosionTextures[i]);
  }

  for (int i = 0; i < 4; i++) {
    if (ctx->exhaustTexture[i])
      SDL_DestroyTexture(ctx->exhaustTexture[i]);
  }

  if (ctx->renderer)
    SDL_DestroyRenderer(ctx->renderer);
  if (ctx->window)
    SDL_DestroyWindow(ctx->window);

  free(ctx);
  TTF_Quit();
  SDL_Quit();
}

// UPDATE: Added 'explosions' parameter
void renderSDL(SDL_Context *ctx, const Player *player,
               const Projectiles *projectiles, const Swarm *swarm,
               const ExplosionManager *explosions) {
  if (!ctx || !player)
    return;

  // 1. Draw Background
  if (ctx->backgroundTexture) {
    SDL_RenderTexture(ctx->renderer, ctx->backgroundTexture, NULL, NULL);
  } else {
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
    SDL_RenderClear(ctx->renderer);
  }

  // 1.5 Draw Player Exhaust (NEW)
  if (player->health > 0) { // Only draw if player is alive
    int frame = player->animFrame;
    if (ctx->exhaustTexture[frame]) {
      // Calculate position: Centered horizontally, directly below the ship
      float fireWidth = 20.0f; // Adjust based on your preference
      float fireHeight = 30.0f;

      SDL_FRect fireRect = {player->x + (player->width / 2.0f) -
                                (fireWidth / 2.0f), // Center X
                            player->y + player->height -
                                5.0f, // Just below player (overlap slightly)
                            fireWidth, fireHeight};

      SDL_RenderTexture(ctx->renderer, ctx->exhaustTexture[frame], NULL,
                        &fireRect);
    }
  }

  // 2. Draw Player
  if (ctx->playerTexture) {
    SDL_FRect playerRect = {player->x, player->y, (float)player->width,
                            (float)player->height};
    SDL_RenderTexture(ctx->renderer, ctx->playerTexture, NULL, &playerRect);
  }

  // 3. Draw Projectiles
  if (projectiles) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
      if (projectiles->projectiles[i].active) {
        SDL_FRect bulletRect = {
            projectiles->projectiles[i].x, projectiles->projectiles[i].y,
            projectiles->projectiles[i].w, projectiles->projectiles[i].h};

        SDL_Texture *bTex = (projectiles->projectiles[i].velocityY < 0)
                                ? ctx->playerProjectileTexture
                                : ctx->enemyProjectileTexture;

        if (bTex) {
          SDL_RenderTexture(ctx->renderer, bTex, NULL, &bulletRect);
        } else {
          SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 0, 255);
          SDL_RenderFillRect(ctx->renderer, &bulletRect);
        }
      }
    }
  }

  // 4. Draw Enemies
  if (swarm) {
    SDL_Texture *currentAlien =
        swarm->animationFrame ? ctx->enemyTexture2 : ctx->enemyTexture1;

    for (int i = 0; i < TOTAL_ENEMIES; i++) {
      if (swarm->enemies[i].active) {
        SDL_FRect enemyRect = {swarm->enemies[i].x, swarm->enemies[i].y,
                               (float)swarm->enemies[i].width,
                               (float)swarm->enemies[i].height};

        if (currentAlien) {
          SDL_RenderTexture(ctx->renderer, currentAlien, NULL, &enemyRect);
        } else {
          SDL_SetRenderDrawColor(ctx->renderer, 255, 0, 0, 255);
          SDL_RenderFillRect(ctx->renderer, &enemyRect);
        }
      }
    }
  }

  // 5. Draw Explosions (NEW SECTION)
  if (explosions) {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
      if (explosions->explosions[i].active) {
        // Get the frame index (0, 1, or 2)
        int frame = explosions->explosions[i].currentFrame;

        // Safety check to ensure we have a valid texture
        if (frame >= 0 && frame < 3 && ctx->explosionTextures[frame]) {
          SDL_FRect explRect = {explosions->explosions[i].x,
                                explosions->explosions[i].y,
                                EXPLOSION_SIZE, // Defined in explosion.h
                                EXPLOSION_SIZE};
          SDL_RenderTexture(ctx->renderer, ctx->explosionTextures[frame], NULL,
                            &explRect);
        }
      }
    }
  }

  // --- 6. HUD: DRAW HEALTH (LIVES) ---
  // Draw the player texture small in the top-left corner
  if (ctx->playerTexture) {
    for (int i = 0; i < player->health; i++) {
      // x = 10 + (i * 35) gives spacing: 10, 45, 80...
      // width/height = 25 (smaller than actual player)
      SDL_FRect lifeRect = {10.0f + (i * 35.0f), 10.0f, 25.0f, 25.0f};
      SDL_RenderTexture(ctx->renderer, ctx->playerTexture, NULL, &lifeRect);
    }
  }

  // --- 7. HUD: DRAW SCORE ---
  if (ctx->font) {
    // Create string
    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "SCORE: %05d", player->score);

    // Render Text to Surface (White color)
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(ctx->font, scoreText, 0, color);

    if (surface) {
      // Convert Surface to Texture
      SDL_Texture *scoreTexture =
          SDL_CreateTextureFromSurface(ctx->renderer, surface);

      if (scoreTexture) {
        // Position at Top Right (approx 200px wide)
        SDL_FRect scoreRect = {(float)(LOGICAL_WIDTH - surface->w - 20), 10.0f,
                               (float)surface->w, (float)surface->h};

        SDL_RenderTexture(ctx->renderer, scoreTexture, NULL, &scoreRect);

        // Texture must be destroyed immediately after use
        SDL_DestroyTexture(scoreTexture);
      }
      SDL_DestroySurface(surface);
    }
  }

  SDL_RenderPresent(ctx->renderer);
}