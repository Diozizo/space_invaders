#include "../includes/controller.h"
#include <SDL3/SDL.h>

bool handleInput(Player *player, Projectiles *projectiles) {
  if (!player)
    return true;

  SDL_Event event;

  // --- 1. Event Loop ---
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      return false;
    }
  }

  // --- 2. Keyboard State ---
  const bool *keyState = SDL_GetKeyboardState(NULL);

  if (keyState[SDL_SCANCODE_ESCAPE]) {
    return false;
  }

  // --- Movement ---
  if (keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A]) {
    setPlayerDirection(player, MOVE_LEFT);
  } else if (keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D]) {
    setPlayerDirection(player, MOVE_RIGHT);
  } else {
    setPlayerDirection(player, MOVE_NONE);
  }

  // --- Shooting (New) ---
  if (keyState[SDL_SCANCODE_SPACE]) {
    playerShoot(player, projectiles);
  }

  return true;
}