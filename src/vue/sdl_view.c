#include "../includes/sdl_view.h"
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// --- CONSTANTS ---
// These define the "Virtual" resolution your game logic runs at.
// Even if the window is 4K, the game thinks it is 800x600.
#define LOGICAL_WIDTH 800
#define LOGICAL_HEIGHT 600

SDL_Context *initSDLView(unsigned windowWidth, unsigned windowHeight) {
  // 1. Force "Nearest Neighbor" scaling
  // This ensures your pixel art stays sharp and doesn't get blurry when
  // stretched.
  SDL_SetHint("SDL_RENDER_SCALE_QUALITY", "nearest");

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return NULL;
  }

  SDL_Context *ctx = (SDL_Context *)malloc(sizeof(SDL_Context));
  if (!ctx)
    return NULL;

  // 2. Create the Physical Window
  // We use the width/height passed from main.c (e.g., 1920x1080).
  ctx->window = SDL_CreateWindow("Space Invaders", windowWidth, windowHeight,
                                 SDL_WINDOW_RESIZABLE);
  if (!ctx->window) {
    printf("Window creation failed: %s\n", SDL_GetError());
    free(ctx);
    return NULL;
  }

  // 3. Create the GPU Renderer
  ctx->renderer = SDL_CreateRenderer(ctx->window, NULL);
  if (!ctx->renderer) {
    printf("Renderer creation failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(ctx->window);
    free(ctx);
    return NULL;
  }

  // 4. Set Logical Presentation (The Magic Step)
  // We tell SDL: "Treat the game as 800x600, but stretch it to fill the actual
  // window."
  if (SDL_SetRenderLogicalPresentation(ctx->renderer, LOGICAL_WIDTH,
                                       LOGICAL_HEIGHT,
                                       SDL_LOGICAL_PRESENTATION_STRETCH) < 0) {
    printf("WARNING: Logical Presentation setup failed: %s\n", SDL_GetError());
  }

  // 5. Store the LOGICAL dimensions
  // This is crucial. The rest of your game (Model/Controller) needs to know
  // the boundaries are 800x600, not the window size.
  ctx->screenWidth = LOGICAL_WIDTH;
  ctx->screenHeight = LOGICAL_HEIGHT;

  return ctx;
}

void destroySDLView(SDL_Context *ctx) {
  if (!ctx)
    return;

  if (ctx->renderer)
    SDL_DestroyRenderer(ctx->renderer);
  if (ctx->window)
    SDL_DestroyWindow(ctx->window);

  free(ctx);
  SDL_Quit();
}

void renderSDL(SDL_Context *ctx, const Player *player,
               const Projectiles *projectiles, const Swarm *swarm) {
  if (!ctx || !player)
    return;

  // 1. Clear Screen (Black)
  SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
  SDL_RenderClear(ctx->renderer);

  // 2. Draw Player (Green)
  // SDL automatically scales these 800x600 coordinates to the window size.
  SDL_FRect playerRect = {player->x, player->y, (float)player->width,
                          (float)player->height};
  SDL_SetRenderDrawColor(ctx->renderer, 0, 255, 0, 255);
  SDL_RenderFillRect(ctx->renderer, &playerRect);

  // 3. Draw Projectiles (Yellow)
  if (projectiles) {
    SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 0, 255);
    for (int i = 0; i < MAX_PROJECTILES; i++) {
      if (projectiles->projectiles[i].active) {
        SDL_FRect bulletRect = {
            projectiles->projectiles[i].x, projectiles->projectiles[i].y,
            projectiles->projectiles[i].w, projectiles->projectiles[i].h};
        SDL_RenderFillRect(ctx->renderer, &bulletRect);
      }
    }
  }

  // 4. Draw Enemies (Red)
  if (swarm) {
    SDL_SetRenderDrawColor(ctx->renderer, 255, 0, 0, 255);
    for (int i = 0; i < TOTAL_ENEMIES; i++) {
      if (swarm->enemies[i].active) {
        SDL_FRect enemyRect = {swarm->enemies[i].x, swarm->enemies[i].y,
                               (float)swarm->enemies[i].width,
                               (float)swarm->enemies[i].height};
        SDL_RenderFillRect(ctx->renderer, &enemyRect);
      }
    }
  }

  // 5. Present (Applies the scaling and shows the frame)
  SDL_RenderPresent(ctx->renderer);
}