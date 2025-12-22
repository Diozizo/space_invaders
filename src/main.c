#include "includes/controller.h"
#include "includes/enemy.h"
#include "includes/explosion.h" // <--- NEW: Include explosion header
#include "includes/game_state.h"
#include "includes/physics.h"
#include "includes/player.h"
#include "includes/projectile.h"
#include "includes/sdl_view.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- CONSTANTS ---
// 1. Physical Window Size (How big the window opens)
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// 2. Logical Game Size (The internal resolution)
// THESE MUST MATCH THE VALUES IN sdl_view.c !!!
#define GAME_WIDTH 800
#define GAME_HEIGHT 600

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  srand((unsigned int)time(NULL));

  GameState state = STATE_MENU;

  // 1. Initialize Player using GAME COORDINATES
  Player *player = createPlayer(GAME_WIDTH / 2.0f, 30, 50);
  if (!player)
    return 1;

  player->y = GAME_HEIGHT - 50;

  // 2. Initialize Projectiles
  Projectiles *bullets = createProjectiles(MAX_PROJECTILES);
  if (!bullets) {
    destroyPlayer(player);
    return 1;
  }

  // 3. Initialize Swarm
  Swarm *swarm = createSwarm();
  if (!swarm) {
    destroyPlayer(player);
    destroyProjectiles(bullets);
    return 1;
  }

  // 4. Initialize Explosions -- NEW --
  ExplosionManager *explosions = createExplosionManager();
  if (!explosions) {
    destroyPlayer(player);
    destroyProjectiles(bullets);
    destroySwarm(swarm);
    return 1;
  }

  // 5. Initialize View
  SDL_Context *view = initSDLView(WINDOW_WIDTH, WINDOW_HEIGHT);
  if (!view) {
    destroyPlayer(player);
    destroyProjectiles(bullets);
    destroySwarm(swarm);
    destroyExplosionManager(
        explosions); // Don't forget to clean this up on error
    return 1;
  }

  bool isRunning = true;
  uint64_t lastTime = SDL_GetTicks();

  // 6. Game Loop
  while (isRunning) {
    uint64_t currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    // A. Input
    isRunning = handleInput(player, bullets, view, &state);

    if (state == STATE_PLAYING) {
      // B. Logic Updates
      updatePlayer(player, deltaTime, GAME_WIDTH);
      updateProjectiles(bullets, deltaTime, GAME_HEIGHT);
      updateSwarm(swarm, deltaTime, GAME_WIDTH);
      enemyAttemptShoot(swarm, bullets, deltaTime);

      // NEW: Update explosion animations
      updateExplosions(explosions, deltaTime);

      // C. Physics (Pass explosions manager so collisions trigger effects)
      if (checkCollisions(player, swarm, bullets, explosions)) {
        printf("GAME OVER - Player Destroyed\n");
        isRunning = false;
      }

      if (isSwarmDestroyed(swarm)) {
        printf("LEVEL COMPLETE - All Enemies Destroyed\n");
        isRunning = false;
      }
    }

    // D. Render (Pass explosions manager to draw them)
    renderSDL(view, player, bullets, swarm, explosions, state);
  }

  // 7. Cleanup
  destroySDLView(view);
  destroyPlayer(player);
  destroyProjectiles(bullets);
  destroySwarm(swarm);
  destroyExplosionManager(explosions); // <--- Destroy explosions

  printf("Game exited cleanly.\n");
  return 0;
}