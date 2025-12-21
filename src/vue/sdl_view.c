#include "../includes/sdl_view.h"
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Context *initSDLView(unsigned width, unsigned height) {
  // FIX: Use the raw string "SDL_RENDER_SCALE_QUALITY" because the constant
  // SDL_HINT_RENDER_SCALE_QUALITY might be missing in your specific SDL3
  // version.
  SDL_SetHint("SDL_RENDER_SCALE_QUALITY", "nearest");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return NULL;
  }

  SDL_Context *ctx = (SDL_Context *)malloc(sizeof(SDL_Context));
  if (!ctx) {
    return NULL;
  }

  // 1. Create Resizable Window
  ctx->window =
      SDL_CreateWindow("Space Invaders", width, height, SDL_WINDOW_RESIZABLE);
  if (!ctx->window) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    free(ctx);
    return NULL;
  }

  ctx->renderer = SDL_CreateRenderer(ctx->window, NULL);
  if (!ctx->renderer) {
    printf("Renderer could no be created! SDL_Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(ctx->window);
    free(ctx);
    return NULL;
  }

  // 2. Set Logical Presentation
  // This handles the resizing and aspect ratio automatically.
  // We use the 4-argument version.
  SDL_SetRenderLogicalPresentation(ctx->renderer, width, height,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);

  ctx->screenHeight = height;
  ctx->screenWidth = width;
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

  // Clear Screen
  SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
  SDL_RenderClear(ctx->renderer);

  // Draw Player
  SDL_FRect playerRect = {player->x, player->y, (float)player->width,
                          (float)player->height};
  SDL_SetRenderDrawColor(ctx->renderer, 0, 255, 0, 255);
  SDL_RenderFillRect(ctx->renderer, &playerRect);

  // Draw Projectiles
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

  // Draw Enemies
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

  SDL_RenderPresent(ctx->renderer);
}