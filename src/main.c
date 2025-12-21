#include "includes/controller.h"
#include "includes/enemy.h"
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

  // 1. Initialize Player using GAME COORDINATES
  // We place him at the bottom of the 600px screen, not the 900px window.
  Player *player = createPlayer(GAME_WIDTH / 2.0f, 30, 50);
  if (!player)
    return 1;

  player->y = GAME_HEIGHT - 50; // Y = 550 (Visible!)

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

  // 4. Initialize View
  // We pass the PHYSICAL size here so the window opens big.
  SDL_Context *view = initSDLView(WINDOW_WIDTH, WINDOW_HEIGHT);
  if (!view) {
    destroyPlayer(player);
    destroyProjectiles(bullets);
    destroySwarm(swarm);
    return 1;
  }

  bool isRunning = true;
  uint64_t lastTime = SDL_GetTicks();

  // 5. Game Loop
  while (isRunning) {
    uint64_t currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    // Input
    isRunning = handleInput(player, bullets);

    // Logic - Use GAME_WIDTH for boundaries
    updatePlayer(player, deltaTime, GAME_WIDTH);
    updateProjectiles(bullets, deltaTime, GAME_HEIGHT);
    updateSwarm(swarm, deltaTime, GAME_WIDTH);

    enemyAttemptShoot(swarm, bullets, deltaTime);

    if (checkCollisions(player, swarm, bullets)) {
      printf("GAME OVER - Player Destroyed\n");
      isRunning = false;
    }

    if (isSwarmDestroyed(swarm)) {
      printf("LEVEL COMPLETE - All Enemies Destroyed\n");
      isRunning = false;
    }

    renderSDL(view, player, bullets, swarm);
  }

  // Cleanup
  destroySDLView(view);
  destroyPlayer(player);
  destroyProjectiles(bullets);
  destroySwarm(swarm);

  printf("Game exited cleanly.\n");
  return 0;
}