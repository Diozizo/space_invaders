#ifndef SDL_VIEW_H
#define SDL_VIEW_H

#include "enemy.h"
#include "explosion.h"
#include "player.h"
#include "projectile.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

// This struct holds the SDL window and renderer so we can pass them around
typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  unsigned screenWidth;
  unsigned screenHeight;

  SDL_Texture *backgroundTexture;
  SDL_Texture *playerTexture;
  SDL_Texture *enemyTexture1;
  SDL_Texture *enemyTexture2;
  SDL_Texture *playerProjectileTexture;
  SDL_Texture *enemyProjectileTexture;

  SDL_Texture *explosionTextures[3];

  SDL_Texture *playerExhaustTexture[4];

  SDL_Texture *exhaustTexture[4];

  TTF_Font *font;
} SDL_Context;

/**
 * @brief Initializes SDL, creates a Window and a Renderer.
 * @return Pointer to context if successful, NULL if error.
 */
SDL_Context *initSDLView(unsigned width, unsigned height);

/**
 * @brief Destroys window and renderer, quits SDL.
 */
void destroySDLView(SDL_Context *ctx);

/**
 * @brief Clears the screen, draws the player AND projectiles, and updates the
 * display.
 */

void toggleFullscreen(SDL_Context *ctx);

void renderSDL(SDL_Context *ctx, const Player *player,
               const Projectiles *projectiles, const Swarm *swarm,
               const ExplosionManager *explosions);

#endif