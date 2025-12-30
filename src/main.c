#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Shared Model Includes
#include "../includes/bunker.h"
#include "../includes/enemy.h"
#include "../includes/explosion.h"
#include "../includes/game_state.h"
#include "../includes/physics.h"
#include "../includes/player.h"
#include "../includes/projectile.h"

// SDL Specific Includes
#include "../includes/controller.h"
#include "../includes/sdl_view.h"
#include <SDL3/SDL.h>

// Ncurses Specific Includes
#include "../includes/ncurses_controller.h"
#include "../includes/ncurses_view.h"

// ==========================================
//               CONSTANTS
// ==========================================
#define GAME_WIDTH 800
#define GAME_HEIGHT 600
#define FPS 60
#define FRAME_DELAY (1000 / FPS) // Target duration per frame (~16ms)

/**
 * @brief Resets the game state for a replay.
 * * When the player dies or wins and hits "Enter", this function:
 * 1. Resets Player position and health.
 * 2. Destroys the old Swarm and Bullet pool.
 * 3. Allocates fresh Swarm and Bullets.
 * 4. Rebuilds the Bunkers.
 * * @note We use double pointers (Swarm **s) because we are modifying
 * the pointer itself (re-allocating memory).
 */
void resetGameLogic(Player *p, Swarm **s, Projectiles **b, BunkerManager *bk,
                    int *lvl) {
  p->x = GAME_WIDTH / 2.0f;
  p->y = GAME_HEIGHT - 50;
  p->health = 3;
  p->score = 0;
  *lvl = 1;

  // Hard Reset: Destroy old objects, create new ones
  destroySwarm(*s);
  *s = createSwarm(*lvl);
  if (!*s) {
    exit(EXIT_FAILURE);
  }

  destroyProjectiles(*b);
  *b = createProjectiles(MAX_PROJECTILES);
  if (!*b) {
    exit(EXIT_FAILURE);
  }

  // Restore shields
  resetBunkers(bk, GAME_WIDTH);
}

// ==========================================
//              SDL RUNNER
// ==========================================
/**
 * @brief The Main Game Loop for the Graphical (SDL) Mode.
 * Handles high-performance rendering, audio, and inputs.
 */
void runSDL() {
  // 1. Initialization Phase
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

  // Time Management for Delta Time
  unsigned long lastTime = SDL_GetTicks();

  // Time Management for Frame Capping
  Uint32 frameStart;
  int frameTime;

  // 2. The Game Loop
  while (isRunning) {
    frameStart = SDL_GetTicks(); // Record the start of this frame

    // A. Calculate Delta Time
    // How many seconds passed since the last frame? (e.g., 0.016s for 60FPS)
    unsigned long currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    // B. INPUT
    // Poll events. If user requests quit (X button), returns false.
    isRunning = handleInput(player, bullets, view, &state);

    // C. UPDATE (Game Logic)
    if (state == STATE_PLAYING) {
      // Move Entities
      updatePlayer(player, deltaTime, GAME_WIDTH);
      updateProjectiles(bullets, deltaTime, GAME_HEIGHT);
      updateSwarm(swarm, deltaTime, GAME_WIDTH);
      updateExplosions(explosions, deltaTime);

      // AI Logic
      if (enemyAttemptShoot(swarm, bullets, deltaTime))
        playSound(view, SOUND_ENEMY_SHOOT);

      // Collision Logic
      bool hit = false;
      if (checkCollisions(player, swarm, bullets, explosions, bunkers, &hit)) {
        // Player Died
        playSound(view, SOUND_PLAYER_EXPLOSION);
        state = STATE_GAME_OVER;
        playerWon = false;
      }
      if (hit)
        playSound(view, SOUND_ENEMY_EXPLOSION);

      // Level Progression
      if (isSwarmDestroyed(swarm)) {
        currentLevel++;
        if (currentLevel > 2) {
          playerWon = true;
          state = STATE_GAME_OVER;
        } else {
          // Next Level: Recreate Swarm, Clear Bullets
          destroySwarm(swarm);
          swarm = createSwarm(currentLevel);
          destroyProjectiles(bullets);
          bullets = createProjectiles(MAX_PROJECTILES);
        }
      }
    }

    // D. RENDER
    renderSDL(view, player, bullets, swarm, explosions, bunkers, state,
              playerWon);

    // E. RESET CHECK
    if (state == STATE_MENU && (player->score > 0 || !player->health)) {
      resetGameLogic(player, &swarm, &bullets, bunkers, &currentLevel);
    }

    // F. FRAME CAPPING (Force 60 FPS)
    frameTime = SDL_GetTicks() - frameStart; // How long did this frame take?

    if (FRAME_DELAY > frameTime) {
      // Wait for the remaining time to reach ~16.6ms
      SDL_Delay(FRAME_DELAY - frameTime);
    }
  }

  // 3. Cleanup Phase (Prevent Memory Leaks)
  printf("Loop exited. Starting cleanup...\n");

  destroySDLView(view);
  destroyPlayer(player);
  destroySwarm(swarm);
  destroyProjectiles(bullets);
  destroyExplosionManager(explosions);
  destroyBunkers(bunkers);

  printf("Cleanup finished successfully.\n");
}

// ==========================================
//            NCURSES RUNNER
// ==========================================
/**
 * @brief The Game Loop for the Terminal (Ncurses) Mode.
 * Uses POSIX clocks for timing since SDL timing functions aren't available
 * here.
 */
void runNcurses() {
  Ncurses_Context *view = initNcursesView(GAME_WIDTH, GAME_HEIGHT);
  if (!view)
    return;

  // Initialize Entities (Same as SDL)
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

  // POSIX Time Setup
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  unsigned long lastTime = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);

  while (isRunning) {
    // A. Time Calculation
    clock_gettime(CLOCK_MONOTONIC, &ts);
    unsigned long currentTime = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    // B. Input
    // needsReset is a flag passed by reference because Ncurses input is handled
    // differently
    isRunning =
        handleNcursesInput(player, bullets, &state, &needsReset, deltaTime);

    // C. Logic
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

      // Level Progression
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
    // Reset Logic specific to Ncurses loop structure
    else if (state == STATE_MENU && needsReset) {
      resetGameLogic(player, &swarm, &bullets, bunkers, &currentLevel);
      needsReset = false;
      playerWon = false;
    }

    // D. Render
    renderNcurses(view, player, bullets, swarm, explosions, bunkers, state,
                  playerWon);

    // E. Throttle
    // Terminals refresh very fast. We sleep briefly to save CPU and reduce
    // flicker.
    struct timespec sleepTs = {0, 15000000}; // 15ms
    nanosleep(&sleepTs, NULL);
  }

  // Cleanup
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
  // Seed the random number generator for enemy shooting variation
  srand((unsigned int)time(NULL));

  // Check command line arguments for mode selection
  if (argc > 1 && strcmp(argv[1], "ncurses") == 0) {
    printf("Mode: NCURSES\n");
    runNcurses();
  } else {
    // Default to SDL (Graphical)
    printf("Mode: SDL\n");
    runSDL();
  }
  return 0;
}