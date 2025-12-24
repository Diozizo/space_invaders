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
#include "includes/controller.h" // Your original SDL controller
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
  destroyProjectiles(*b);
  *b = createProjectiles(MAX_PROJECTILES);
  resetBunkers(bk, GAME_WIDTH);
}

// ==========================================
//              SDL RUNNER
// ==========================================
void runSDL() {
  SDL_Context *view = initSDLView(GAME_WIDTH, GAME_HEIGHT);
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
  // We don't use a needsReset flag here because your old controller handles
  // transitions differently, but we can detect state changes.

  unsigned long lastTime = SDL_GetTicks();

  while (isRunning) {
    unsigned long currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    // 1. INPUT (Uses your original controller)
    // This function sets player direction (MOVE_LEFT/MOVE_RIGHT) but doesn't
    // move X directly
    isRunning = handleInput(player, bullets, view, &state);

    // 2. LOGIC
    if (state == STATE_PLAYING) {
      // CRITICAL: updatePlayer applies the movement based on the direction set
      // by handleInput
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
    } else if (state == STATE_GAME_OVER) {
      // Simple check if user pressed Enter to restart (detected in handleInput
      // setting state to MENU) If we just transitioned to MENU, we reset. Note:
      // Your handleInput sets state to MENU on Enter. We detect that here:
      // Actually, since handleInput changes state, we need to check if we need
      // to reset *after* the change. We'll rely on a manual check: if player
      // score > 0 and state is MENU, assume restart.
    }

    // 3. RENDER
    renderSDL(view, player, bullets, swarm, explosions, bunkers, state,
              playerWon);

    // Manual Reset Check (Since your controller sets state directly)
    if (state == STATE_MENU && (player->score > 0 || !player->health)) {
      resetGameLogic(player, &swarm, &bullets, bunkers, &currentLevel);
    }
  }

  destroySDLView(view);
  destroyPlayer(player);
  destroySwarm(swarm);
  destroyProjectiles(bullets);
  destroyExplosionManager(explosions);
  destroyBunkers(bunkers);
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

      // --- FIX: DECREMENT SHOOT TIMER ---
      // This allows the player to shoot again after the cooldown
      if (player->shootTimer > 0.0f) {
        player->shootTimer -= deltaTime;
      }

      // Update entities
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

    // 3. RENDER
    renderNcurses(view, player, bullets, swarm, explosions, bunkers, state,
                  playerWon);

    struct timespec sleepTs = {0, 15000000}; // 15ms sleep
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