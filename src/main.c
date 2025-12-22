#include "includes/bunker.h"
#include "includes/controller.h"
#include "includes/enemy.h"
#include "includes/explosion.h"
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
// 1. Physical Window Size
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// 2. Logical Game Size
#define GAME_WIDTH 800
#define GAME_HEIGHT 600

// Helper to reset everything for a new game
void resetGame(Player *player, Swarm **swarm, Projectiles **bullets,
               BunkerManager *bunkers, int *currentLevel) {
  // 1. Reset Player
  player->x = GAME_WIDTH / 2.0f;
  player->y = GAME_HEIGHT - 50;
  player->health = 3;
  player->score = 0;
  player->animFrame = 0;
  player->shootTimer = 0.0f;

  // 2. Reset Level
  *currentLevel = 1;

  // 3. Re-create Swarm (Level 1)
  destroySwarm(*swarm);
  *swarm = createSwarm();

  // 4. Re-create Projectiles (Clears all active bullets)
  destroyProjectiles(*bullets);
  *bullets = createProjectiles(MAX_PROJECTILES);

  resetBunkers(bunkers, GAME_WIDTH);

  printf("Game Reset!\n");
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  srand((unsigned int)time(NULL));

  // --- INITIALIZATION ---

  BunkerManager *bunkers = createBunkers(GAME_WIDTH);
  if (!bunkers) {
    return 1;
  }

  // 1. Player
  Player *player = createPlayer(GAME_WIDTH / 2.0f, 30, 50);
  if (!player) {
    destroyBunkers(bunkers);
    return 1;
  }
  player->y = GAME_HEIGHT - 50;

  // 2. Projectiles
  Projectiles *bullets = createProjectiles(MAX_PROJECTILES);
  if (!bullets) {
    destroyBunkers(bunkers);
    destroyPlayer(player);
    return 1;
  }

  // 3. Swarm (Start at Level 1)
  int currentLevel = 1;
  Swarm *swarm = createSwarm();
  if (!swarm) {
    destroyBunkers(bunkers);
    destroyPlayer(player);
    destroyProjectiles(bullets);
    return 1;
  }

  // 4. Explosions
  ExplosionManager *explosions = createExplosionManager();
  if (!explosions) {
    destroyBunkers(bunkers);
    destroyPlayer(player);
    destroyProjectiles(bullets);
    destroySwarm(swarm);
    return 1;
  }

  // 5. View
  SDL_Context *view = initSDLView(WINDOW_WIDTH, WINDOW_HEIGHT);
  if (!view) {
    destroyBunkers(bunkers);
    destroyPlayer(player);
    destroyProjectiles(bullets);
    destroySwarm(swarm);
    destroyExplosionManager(explosions);
    return 1;
  }

  // --- STATE MANAGEMENT ---
  GameState state = STATE_MENU;
  bool isRunning = true;

  // Flags for Game Over logic
  bool playerWon = false;
  bool needsReset = false;

  uint64_t lastTime = SDL_GetTicks();

  // 6. Game Loop
  while (isRunning) {
    uint64_t currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    // --- RESET CHECK ---
    // If we are back in MENU after a game over, reset the world
    if (state == STATE_MENU && needsReset) {
      resetGame(player, &swarm, &bullets, bunkers, &currentLevel);
      playerWon = false;
      needsReset = false;
    }

    // A. Input
    isRunning = handleInput(player, bullets, view, &state);

    if (state == STATE_PLAYING) {
      // B. Logic Updates
      updatePlayer(player, deltaTime, GAME_WIDTH);
      updateProjectiles(bullets, deltaTime, GAME_HEIGHT);
      updateSwarm(swarm, deltaTime, GAME_WIDTH);
      enemyAttemptShoot(swarm, bullets, deltaTime);
      updateExplosions(explosions, deltaTime);

      // C. Physics
      if (checkCollisions(player, swarm, bullets, explosions, bunkers)) {
        printf("GAME OVER - Player Destroyed\n");
        playerWon = false; // Loss
        needsReset = true; // Schedule reset
        state = STATE_GAME_OVER;
      }

      // D. Level Progression
      if (isSwarmDestroyed(swarm)) {
        printf("LEVEL %d COMPLETE\n", currentLevel);
        currentLevel++;

        if (currentLevel > 3) {
          // VICTORY (Beat Level 3/Boss)
          printf("YOU WIN!\n");
          playerWon = true;  // Win
          needsReset = true; // Schedule reset
          state = STATE_GAME_OVER;
        } else {
          // NEXT LEVEL
          destroySwarm(swarm);
          swarm = createSwarm();

          // Clean bullets for fairness
          destroyProjectiles(bullets);
          bullets = createProjectiles(MAX_PROJECTILES);
        }
      }
    }

    // E. Render
    // We pass 'playerWon' so the view knows to draw Green or Red text
    renderSDL(view, player, bullets, swarm, explosions, bunkers, state,
              playerWon);
  }

  // 7. Cleanup
  destroySDLView(view);
  destroyPlayer(player);
  destroyProjectiles(bullets);
  destroySwarm(swarm);
  destroyExplosionManager(explosions);
  destroyBunkers(bunkers);

  printf("Game exited cleanly.\n");
  return 0;
}