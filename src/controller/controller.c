#include "../includes/controller.h"
#include <SDL3/SDL.h>

bool handleInput(Player *player, Projectiles *projectiles, SDL_Context *view,
                 GameState *state) {
  if (!player)
    return true;

  SDL_Event event;

  // --- 1. Event Loop ---
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      return false;
    }

    if (event.type == SDL_EVENT_KEY_DOWN) {
      // Toggle Fullscreen on 'F'
      if (event.key.scancode == SDL_SCANCODE_F) {
        toggleFullscreen(view);
      }

      // --- MENU STATE ---
      if (*state == STATE_MENU) {
        if (event.key.scancode == SDL_SCANCODE_RETURN) {
          *state = STATE_PLAYING; // Start Game
        }
        if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
          return false; // Quit App
        }
      }

      // --- PLAYING STATE ---
      else if (*state == STATE_PLAYING) {
        if (event.key.scancode == SDL_SCANCODE_P ||
            event.key.scancode == SDL_SCANCODE_ESCAPE) {
          *state = STATE_PAUSED; // Pause
        }
      }

      // --- PAUSED STATE ---
      else if (*state == STATE_PAUSED) {
        if (event.key.scancode == SDL_SCANCODE_P ||
            event.key.scancode == SDL_SCANCODE_ESCAPE) {
          *state = STATE_PLAYING; // Resume
        }
      }

      // --- GAME OVER STATE ---
      else if (*state == STATE_GAME_OVER) {
        if (event.key.scancode == SDL_SCANCODE_RETURN) {
          // In main.c, we will detect this change and reset the game entities
          *state = STATE_MENU;
        }
      }
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