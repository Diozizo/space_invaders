#include "includes/controller.h"
#include "includes/enemy.h"
#include "includes/physics.h"
#include "includes/player.h"
#include "includes/projectile.h"
#include "includes/sdl_view.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h> // Required for srand()
#include <time.h>   // Required for time()

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  // 0. Seed Random Number Generator (Crucial for Enemy Shooting)
  srand((unsigned int)time(NULL));

  // 1. Initialize Player
  Player *player = createPlayer(SCREEN_WIDTH / 2.0f, 30, 50);
  if (!player)
    return 1;
  player->y = SCREEN_HEIGHT - 50;

  // 2. Initialize Projectiles
  Projectiles *bullets = createProjectiles(MAX_PROJECTILES);
  if (!bullets) {
    destroyPlayer(player);
    return 1;
  }

  // 3. Initialize Swarm (Enemies)
  Swarm *swarm = createSwarm();
  if (!swarm) {
    destroyPlayer(player);
    destroyProjectiles(bullets);
    return 1;
  }

  // 4. Initialize View
  SDL_Context *view = initSDLView(SCREEN_WIDTH, SCREEN_HEIGHT);
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
    // A. Time Management
    uint64_t currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    // B. Input
    isRunning = handleInput(player, bullets);

    // C. Logic (Model)
    updatePlayer(player, deltaTime, SCREEN_WIDTH);
    updateProjectiles(bullets, deltaTime, SCREEN_HEIGHT);
    updateSwarm(swarm, deltaTime, SCREEN_WIDTH);

    // Attempt to make enemies shoot
    enemyAttemptShoot(swarm, bullets, deltaTime);

    // D. Physics / Collisions -- NEW --
    // Checks if bullets hit enemies or if player got hit
    if (checkCollisions(player, swarm, bullets)) {
      printf("GAME OVER - Player Destroyed\n");
      isRunning = false; // Stop the game loop
    }

    // E. Render (View)
    renderSDL(view, player, bullets, swarm);
  }

  // 6. Cleanup
  destroySDLView(view);
  destroyPlayer(player);
  destroyProjectiles(bullets);
  destroySwarm(swarm);

  printf("Game exited cleanly.\n");
  return 0;
}