#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Shared Includes
#include "includes/bunker.h"
#include "includes/enemy.h"
#include "includes/explosion.h"
#include "includes/game_state.h"
#include "includes/physics.h"
#include "includes/player.h"
#include "includes/projectile.h"

// SDL Specifics
#include "includes/controller.h"
#include "includes/sdl_view.h"
#include <SDL3/SDL.h>

// Ncurses Specifics
#include "includes/ncurses_controller.h"
#include "includes/ncurses_view.h"

#define GAME_WIDTH 800
#define GAME_HEIGHT 600

// Helper to reset the game world
void resetGameLogic(Player *p, Swarm **s, Projectiles **b, BunkerManager *bk,
                    int *lvl) {
  p->x = GAME_WIDTH / 2.0f;
  p->y = GAME_HEIGHT - 50;
  p->health = 3;
  p->score = 0;
  *lvl = 1;
  destroySwarm(*s);
  *s = createSwarm(*lvl);
  if (!s) {
    exit(EXIT_FAILURE);
  }
  destroyProjectiles(*b);
  *b = createProjectiles(MAX_PROJECTILES);
  if (!b) {
    exit(EXIT_FAILURE);
  }
  resetBunkers(bk, GAME_WIDTH);
}

// ==========================================
//              SDL RUNNER
// ==========================================
void runSDL() {
  printf("DEBUG: Initializing SDL View...\n");
  SDL_Context *view = initSDLView(GAME_WIDTH, GAME_HEIGHT);
  if (!view)
    return;

  Player *player = createPlayer(GAME_WIDTH / 2.0f, 30, 50);
  if (!player) {
    destroySDLView(view);
    return;
  }
  player->y = GAME_HEIGHT - 50;
  Swarm *swarm = createSwarm(1);
  if (!swarm) {
    destroyPlayer(player);
    destroySDLView(view);
    return;
  }
  Projectiles *bullets = createProjectiles(MAX_PROJECTILES);
  if (!bullets) {
    destroySwarm(swarm);
    destroyPlayer(player);
    destroySDLView(view);
    return;
  }
  ExplosionManager *explosions = createExplosionManager();
  if (!explosions) {
    destroyProjectiles(bullets);
    destroySwarm(swarm);
    destroyPlayer(player);
    destroySDLView(view);
    return;
  }
  BunkerManager *bunkers = createBunkers(GAME_WIDTH);
  if (!bunkers) {
    destroyExplosionManager(explosions);
    destroyProjectiles(bullets);
    destroySwarm(swarm);
    destroyPlayer(player);
    destroySDLView(view);
    return;
  }
  int currentLevel = 1;

  GameState state = STATE_MENU;
  bool isRunning = true;
  bool playerWon = false;

  unsigned long lastTime = SDL_GetTicks();

  while (isRunning) {
    unsigned long currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    // 1. INPUT
    // If you click X, handleInput must return FALSE
    isRunning = handleInput(player, bullets, view, &state);

    // 2. LOGIC
    if (state == STATE_PLAYING) {
      updatePlayer(player, deltaTime, GAME_WIDTH);
      updateProjectiles(bullets, deltaTime, GAME_HEIGHT);
      updateSwarm(swarm, deltaTime, GAME_WIDTH);
      updateExplosions(explosions, deltaTime);

      if (enemyAttemptShoot(swarm, bullets, deltaTime))
        playSound(view, SOUND_ENEMY_SHOOT);

      bool hit = false;
      if (checkCollisions(player, swarm, bullets, explosions, bunkers, &hit)) {
        playSound(view, SOUND_PLAYER_EXPLOSION);
        state = STATE_GAME_OVER;
        playerWon = false;
      }
      if (hit)
        playSound(view, SOUND_ENEMY_EXPLOSION);

      if (isSwarmDestroyed(swarm)) {
        currentLevel++;
        if (currentLevel > 2) {
          playerWon = true;
          state = STATE_GAME_OVER;
        } else {
          destroySwarm(swarm);
          swarm = createSwarm(currentLevel);
          destroyProjectiles(bullets);
          bullets = createProjectiles(MAX_PROJECTILES);
        }
      }
    }

    // 3. RENDER
    renderSDL(view, player, bullets, swarm, explosions, bunkers, state,
              playerWon);

    // Manual Reset Check
    if (state == STATE_MENU && (player->score > 0 || !player->health)) {
      resetGameLogic(player, &swarm, &bullets, bunkers, &currentLevel);
    }
  }

  // --- DEBUG PRINTS TO PROVE CLEANUP RUNS ---
  printf("DEBUG: Loop exited. Starting cleanup...\n");

  destroySDLView(view);
  destroyPlayer(player);
  destroySwarm(swarm);
  destroyProjectiles(bullets);
  destroyExplosionManager(explosions);
  destroyBunkers(bunkers);

  printf("DEBUG: Cleanup finished successfully.\n");
}

// ==========================================
//            NCURSES RUNNER
// ==========================================
void runNcurses() {
  Ncurses_Context *view = initNcursesView(GAME_WIDTH, GAME_HEIGHT);
  if (!view)
    return;

  Player *player = createPlayer(GAME_WIDTH / 2.0f, 30, 50);
  player->y = GAME_HEIGHT - 50;
  Swarm *swarm = createSwarm(1);
  Projectiles *bullets = createProjectiles(MAX_PROJECTILES);
  ExplosionManager *explosions = createExplosionManager();
  BunkerManager *bunkers = createBunkers(GAME_WIDTH);
  int currentLevel = 1;

  GameState state = STATE_MENU;
  bool isRunning = true;
  bool playerWon = false;
  bool needsReset = false;

  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  unsigned long lastTime = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);

  while (isRunning) {
    clock_gettime(CLOCK_MONOTONIC, &ts);
    unsigned long currentTime = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    // 1. INPUT
    isRunning =
        handleNcursesInput(player, bullets, &state, &needsReset, deltaTime);

    // 2. LOGIC
    if (state == STATE_PLAYING) {
      if (player->shootTimer > 0.0f) {
        player->shootTimer -= deltaTime;
      }

      updateProjectiles(bullets, deltaTime, GAME_HEIGHT);
      updateSwarm(swarm, deltaTime, GAME_WIDTH);
      updateExplosions(explosions, deltaTime);
      enemyAttemptShoot(swarm, bullets, deltaTime);

      bool hit = false;
      if (checkCollisions(player, swarm, bullets, explosions, bunkers, &hit)) {
        state = STATE_GAME_OVER;
        playerWon = false;
      }

      if (isSwarmDestroyed(swarm)) {
        currentLevel++;
        if (currentLevel > 2) {
          playerWon = true;
          state = STATE_GAME_OVER;
        } else {
          destroySwarm(swarm);
          swarm = createSwarm(currentLevel);
          destroyProjectiles(bullets);
          bullets = createProjectiles(MAX_PROJECTILES);
        }
      }
    } else if (state == STATE_MENU && needsReset) {
      resetGameLogic(player, &swarm, &bullets, bunkers, &currentLevel);
      needsReset = false;
      playerWon = false;
    }

    renderNcurses(view, player, bullets, swarm, explosions, bunkers, state,
                  playerWon);

    struct timespec sleepTs = {0, 15000000};
    nanosleep(&sleepTs, NULL);
  }

  destroyNcursesView(view);
  destroyPlayer(player);
  destroySwarm(swarm);
  destroyProjectiles(bullets);
  destroyExplosionManager(explosions);
  destroyBunkers(bunkers);
}

// ==========================================
//               ENTRY POINT
// ==========================================
int main(int argc, char *argv[]) {
  srand((unsigned int)time(NULL));

  if (argc > 1 && strcmp(argv[1], "ncurses") == 0) {
    printf("Mode: NCURSES\n");
    runNcurses();
  } else {
    printf("Mode: SDL\n");
    runSDL();
  }
  return 0;
}