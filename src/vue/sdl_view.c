#include "../../includes/sdl_view.h"
#include "../../includes/explosion.h"
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

#define LOGICAL_WIDTH 800
#define LOGICAL_HEIGHT 600

// Helper to reduce repetitive code and add error logging
SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *path) {
  SDL_Texture *tex = IMG_LoadTexture(renderer, path);
  if (!tex) {
    printf("Failed to load texture '%s': %s\n", path, SDL_GetError());
  }
  return tex;
}

SDL_Context *initSDLView(unsigned windowWidth, unsigned windowHeight) {
  // --- 1. Initialize SDL Subsystems ---
  SDL_SetHint("SDL_RENDER_SCALE_QUALITY",
              "nearest"); // Pixel art look (no blur)

  // Initialize Video (Graphics), Events (Input), and Audio
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO)) {
    printf("SDL Init Error: %s\n", SDL_GetError());
    return NULL;
  }

  // Initialize Audio Mixer
  if (!MIX_Init()) {
    printf("MIX Init Error: %s\n", SDL_GetError());
    SDL_Quit();
    return NULL;
  }

  // Create Audio Device
  MIX_Mixer *mixer =
      MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
  if (!mixer) {
    printf("MIX Create Mixer Error: %s\n", SDL_GetError());
    MIX_Quit();
    SDL_Quit();
    return NULL;
  }

  // Initialize Font Engine
  if (!TTF_Init()) {
    printf("TTF Init Error: %s\n", SDL_GetError());
    SDL_Quit();
    return NULL;
  }

  // Allocate Context Memory
  SDL_Context *ctx = (SDL_Context *)calloc(1, sizeof(SDL_Context));
  if (!ctx)
    return NULL;

  ctx->mixer = mixer;

  // --- 2. Create Window & Renderer ---
  ctx->window = SDL_CreateWindow("Space Invaders", windowWidth, windowHeight,
                                 SDL_WINDOW_RESIZABLE);
  if (!ctx->window) {
    free(ctx);
    return NULL;
  }

  ctx->renderer = SDL_CreateRenderer(ctx->window, NULL);
  if (!ctx->renderer) {
    SDL_DestroyWindow(ctx->window);
    free(ctx);
    return NULL;
  }

  // --- 3. Setup Logical Scaling ---
  // This is critical: We tell SDL to treat the window as 800x600 logical units,
  // regardless of the actual window size (e.g., 1920x1080).
  // This handles black bars (letterboxing) automatically.
  if (SDL_SetRenderLogicalPresentation(ctx->renderer, LOGICAL_WIDTH,
                                       LOGICAL_HEIGHT,
                                       SDL_LOGICAL_PRESENTATION_STRETCH) < 0) {
    printf("Warning: Logical Presentation failed: %s\n", SDL_GetError());
  }
  ctx->screenWidth = LOGICAL_WIDTH;
  ctx->screenHeight = LOGICAL_HEIGHT;

  // --- 4. Load Fonts & Textures ---
  ctx->font = TTF_OpenFont("assets/font.ttf", 24);
  if (!ctx->font) {
    printf("WARNING: Failed to load font: %s\n", SDL_GetError());
  }

  ctx->backgroundTexture = loadTexture(ctx->renderer, "assets/background.png");
  ctx->playerTexture = loadTexture(ctx->renderer, "assets/player.png");
  ctx->enemyTexture1 = loadTexture(ctx->renderer, "assets/alien_1.png");
  ctx->enemyTexture2 = loadTexture(ctx->renderer, "assets/alien_2.png");
  ctx->playerProjectileTexture =
      loadTexture(ctx->renderer, "assets/bullet_1.png");
  ctx->enemyProjectileTexture =
      loadTexture(ctx->renderer, "assets/bullet_2.png");

  // Load Animation Frames (Arrays)
  ctx->exhaustTexture[0] =
      loadTexture(ctx->renderer, "assets/player_fire_1.png");
  ctx->exhaustTexture[1] =
      loadTexture(ctx->renderer, "assets/player_fire_2.png");
  ctx->exhaustTexture[2] =
      loadTexture(ctx->renderer, "assets/player_fire_3.png");
  ctx->exhaustTexture[3] =
      loadTexture(ctx->renderer, "assets/player_fire_4.png");

  ctx->bunkerTexture = loadTexture(ctx->renderer, "assets/bunker.png");
  ctx->bossTexture = loadTexture(ctx->renderer, "assets/boss.png");

  ctx->explosionTextures[0] =
      loadTexture(ctx->renderer, "assets/explosion_1.png");
  ctx->explosionTextures[1] =
      loadTexture(ctx->renderer, "assets/explosion_2.png");
  ctx->explosionTextures[2] =
      loadTexture(ctx->renderer, "assets/explosion_3.png");

  // --- 5. Load Audio ---
  ctx->backgroundMusic =
      MIX_LoadAudio(ctx->mixer, "assets/melody.wav", false); // Streamed

  if (!ctx->backgroundMusic) {
    printf("Failed to load melody.wav: %s\n", SDL_GetError());
    ctx->musicTrack = NULL;
  } else {
    // Start Music Loop immediately
    ctx->musicTrack = MIX_CreateTrack(ctx->mixer);
    if (ctx->musicTrack) {
      MIX_SetTrackAudio(ctx->musicTrack, ctx->backgroundMusic);
      MIX_SetTrackGain(ctx->musicTrack, 0.5f); // 50% Volume

      SDL_PropertiesID props = SDL_CreateProperties();
      SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER,
                            -1); // Infinite Loop
      MIX_PlayTrack(ctx->musicTrack, props);
      SDL_DestroyProperties(props);
    }

    // Load Sound Effects (Memory Chunks)
    ctx->sfxPlayerShoot = MIX_LoadAudio(ctx->mixer, "assets/shoot.wav", true);
    ctx->sfxEnemyShoot =
        MIX_LoadAudio(ctx->mixer, "assets/enemy_shoot.wav", true);
    ctx->sfxEnemyExplosion =
        MIX_LoadAudio(ctx->mixer, "assets/explosion.wav", true);
    ctx->sfxPlayerExplosion =
        MIX_LoadAudio(ctx->mixer, "assets/player_explosion.wav", true);
  }

  // Safety Check
  if (!ctx->playerTexture || !ctx->enemyTexture1) {
    printf("CRITICAL: Failed to load game assets. Check file paths!\n");
  }

  return ctx;
}

void destroySDLView(SDL_Context *ctx) {
  if (!ctx)
    return;

  // --- 1. Destroy Textures ---
  if (ctx->backgroundTexture)
    SDL_DestroyTexture(ctx->backgroundTexture);
  if (ctx->playerTexture)
    SDL_DestroyTexture(ctx->playerTexture);
  if (ctx->enemyTexture1)
    SDL_DestroyTexture(ctx->enemyTexture1);
  if (ctx->enemyTexture2)
    SDL_DestroyTexture(ctx->enemyTexture2);
  if (ctx->playerProjectileTexture)
    SDL_DestroyTexture(ctx->playerProjectileTexture);
  if (ctx->enemyProjectileTexture)
    SDL_DestroyTexture(ctx->enemyProjectileTexture);
  if (ctx->bossTexture)
    SDL_DestroyTexture(ctx->bossTexture);
  if (ctx->bunkerTexture)
    SDL_DestroyTexture(ctx->bunkerTexture);

  // Clean Arrays
  for (int i = 0; i < 3; i++) {
    if (ctx->explosionTextures[i])
      SDL_DestroyTexture(ctx->explosionTextures[i]);
  }
  for (int i = 0; i < 4; i++) {
    if (ctx->exhaustTexture[i])
      SDL_DestroyTexture(ctx->exhaustTexture[i]);
    if (ctx->playerExhaustTexture[i])
      SDL_DestroyTexture(ctx->playerExhaustTexture[i]);
  }

  // --- 2. Destroy Audio ---
  if (ctx->musicTrack)
    MIX_DestroyTrack(ctx->musicTrack);
  if (ctx->backgroundMusic)
    MIX_DestroyAudio(ctx->backgroundMusic);

  if (ctx->sfxPlayerShoot)
    MIX_DestroyAudio(ctx->sfxPlayerShoot);
  if (ctx->sfxEnemyExplosion)
    MIX_DestroyAudio(ctx->sfxEnemyExplosion);
  if (ctx->sfxPlayerExplosion)
    MIX_DestroyAudio(ctx->sfxPlayerExplosion);
  if (ctx->sfxEnemyShoot)
    MIX_DestroyAudio(ctx->sfxEnemyShoot);

  if (ctx->mixer)
    MIX_DestroyMixer(ctx->mixer);

  // --- 3. Destroy Font & Window ---
  if (ctx->font)
    TTF_CloseFont(ctx->font);
  if (ctx->renderer)
    SDL_DestroyRenderer(ctx->renderer);
  if (ctx->window)
    SDL_DestroyWindow(ctx->window);

  // --- 4. Quit Subsystems ---
  MIX_Quit();
  TTF_Quit();
  SDL_Quit();

  free(ctx);
}

// Helper to center and render text at a specific Y coordinate
void renderText(SDL_Context *ctx, const char *text, int y, SDL_Color color) {
  if (!ctx->font)
    return;

  SDL_Surface *surface = TTF_RenderText_Solid(ctx->font, text, 0, color);
  if (!surface)
    return;

  SDL_Texture *texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
  if (texture) {
    int textW = surface->w;
    int textH = surface->h;
    // Calculate X to center the text
    int x = (ctx->screenWidth - textW) / 2;

    SDL_FRect rect = {(float)x, (float)y, (float)textW, (float)textH};
    SDL_RenderTexture(ctx->renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
  }
  SDL_DestroySurface(surface);
}

void playSound(SDL_Context *ctx, SoundEffect effect) {
  if (!ctx || !ctx->mixer) {
    return;
  }

  // Map Enum to Audio Pointer
  MIX_Audio *target = NULL;
  switch (effect) {
  case SOUND_PLAYER_SHOOT:
    target = ctx->sfxPlayerShoot;
    break;
  case SOUND_ENEMY_SHOOT:
    target = ctx->sfxEnemyShoot;
    break;
  case SOUND_ENEMY_EXPLOSION:
    target = ctx->sfxEnemyExplosion;
    break;
  case SOUND_PLAYER_EXPLOSION:
    target = ctx->sfxPlayerExplosion;
    break;
  }

  if (target) {
    MIX_PlayAudio(ctx->mixer, target);
  }
}

void renderSDL(SDL_Context *ctx, const Player *player,
               const Projectiles *projectiles, const Swarm *swarm,
               const ExplosionManager *explosions, const BunkerManager *bunkers,
               GameState gameState, bool playerWon) {
  if (!ctx || !player)
    return;

  // --- LAYER 0: BACKGROUND ---
  if (ctx->backgroundTexture) {
    SDL_RenderTexture(ctx->renderer, ctx->backgroundTexture, NULL, NULL);
  } else {
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
    SDL_RenderClear(ctx->renderer);
  }

  // --- LAYER 1: GAMEPLAY ENTITIES ---
  // (Only visible in PLAYING, PAUSED, and GAME OVER states)
  if (gameState == STATE_PLAYING || gameState == STATE_PAUSED ||
      gameState == STATE_GAME_OVER) {

    // A. Player Exhaust (Engine Particle)
    if (player->health > 0) {
      int frame = player->animFrame;
      if (ctx->exhaustTexture[frame]) {
        float fireWidth = 20.0f;
        float fireHeight = 30.0f;

        // Position fire below the ship
        SDL_FRect fireRect = {
            player->x + (player->width / 2.0f) - (fireWidth / 2.0f),
            player->y + player->height - 5.0f, fireWidth, fireHeight};

        SDL_RenderTexture(ctx->renderer, ctx->exhaustTexture[frame], NULL,
                          &fireRect);
      }
    }

    // B. Player Sprite
    if (ctx->playerTexture) {
      SDL_FRect playerRect = {player->x, player->y, (float)player->width,
                              (float)player->height};
      SDL_RenderTexture(ctx->renderer, ctx->playerTexture, NULL, &playerRect);
    }

    // C. Projectiles
    if (projectiles) {
      for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles->projectiles[i].active) {
          SDL_FRect bulletRect = {
              projectiles->projectiles[i].x, projectiles->projectiles[i].y,
              projectiles->projectiles[i].w, projectiles->projectiles[i].h};

          // Choose texture based on velocity (Up=Player, Down=Enemy)
          SDL_Texture *bTex = (projectiles->projectiles[i].velocityY < 0)
                                  ? ctx->playerProjectileTexture
                                  : ctx->enemyProjectileTexture;

          if (bTex) {
            SDL_RenderTexture(ctx->renderer, bTex, NULL, &bulletRect);
          } else {
            SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 0,
                                   255); // Fallback Yellow
            SDL_RenderFillRect(ctx->renderer, &bulletRect);
          }
        }
      }
    }

    // D. Bunkers (Shields)
    if (bunkers) {
      for (int b = 0; b < BUNKER_COUNT; b++) {
        int totalBlocks = BUNKER_ROWS * BUNKER_COLS;
        for (int i = 0; i < totalBlocks; i++) {
          // Only draw active blocks
          if (bunkers->bunkers[b].blocks[i].active) {
            SDL_FRect blockRect = {bunkers->bunkers[b].blocks[i].x,
                                   bunkers->bunkers[b].blocks[i].y, BLOCK_SIZE,
                                   BLOCK_SIZE};

            if (ctx->bunkerTexture) {
              SDL_RenderTexture(ctx->renderer, ctx->bunkerTexture, NULL,
                                &blockRect);
            } else {
              SDL_SetRenderDrawColor(ctx->renderer, 0, 255, 0,
                                     255); // Fallback Green
              SDL_RenderFillRect(ctx->renderer, &blockRect);
            }
          }
        }
      }
    }

    // E. Enemies / Boss
    if (swarm) {

      // --- BOSS MODE (Level 2) ---
      if (swarm->level == 2 && swarm->boss.active) {
        SDL_FRect bossRect = {swarm->boss.x, swarm->boss.y, swarm->boss.width,
                              swarm->boss.height};

        if (ctx->bossTexture) {
          SDL_RenderTexture(ctx->renderer, ctx->bossTexture, NULL, &bossRect);
        } else {
          SDL_SetRenderDrawColor(ctx->renderer, 255, 0, 0, 255);
          SDL_RenderFillRect(ctx->renderer, &bossRect);
        }

        // Draw Boss Health Bar (Above Boss)
        float hpPercent =
            (float)swarm->boss.health / (float)swarm->boss.maxHealth;

        SDL_SetRenderDrawColor(ctx->renderer, 255, 0, 0, 255); // Red Background
        SDL_FRect hpBg = {swarm->boss.x, swarm->boss.y - 15, swarm->boss.width,
                          10};
        SDL_RenderFillRect(ctx->renderer, &hpBg);

        SDL_SetRenderDrawColor(ctx->renderer, 0, 255, 0,
                               255); // Green Foreground
        SDL_FRect hpFg = {swarm->boss.x, swarm->boss.y - 15,
                          swarm->boss.width * hpPercent, 10};
        SDL_RenderFillRect(ctx->renderer, &hpFg);
      }
      // --- SWARM MODE (Level 1) ---
      else {
        // Toggle texture for animation effect
        SDL_Texture *currentAlien =
            swarm->animationFrame ? ctx->enemyTexture2 : ctx->enemyTexture1;

        for (int i = 0; i < TOTAL_ENEMIES; i++) {
          if (swarm->enemies[i].active) {
            SDL_FRect enemyRect = {swarm->enemies[i].x, swarm->enemies[i].y,
                                   (float)swarm->enemies[i].width,
                                   (float)swarm->enemies[i].height};

            if (currentAlien) {
              SDL_RenderTexture(ctx->renderer, currentAlien, NULL, &enemyRect);
            } else {
              SDL_SetRenderDrawColor(ctx->renderer, 255, 0, 0, 255);
              SDL_RenderFillRect(ctx->renderer, &enemyRect);
            }
          }
        }
      }
    }

    // F. Explosions
    if (explosions) {
      for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions->explosions[i].active) {
          int frame = explosions->explosions[i].currentFrame;
          // Ensure frame index is valid
          if (frame >= 0 && frame < 3 && ctx->explosionTextures[frame]) {
            SDL_FRect explRect = {explosions->explosions[i].x,
                                  explosions->explosions[i].y, EXPLOSION_SIZE,
                                  EXPLOSION_SIZE};
            SDL_RenderTexture(ctx->renderer, ctx->explosionTextures[frame],
                              NULL, &explRect);
          }
        }
      }
    }

    // G. HUD: Lives
    if (ctx->playerTexture) {
      for (int i = 0; i < player->health; i++) {
        SDL_FRect lifeRect = {10.0f + (i * 35.0f), 10.0f, 25.0f, 25.0f};
        SDL_RenderTexture(ctx->renderer, ctx->playerTexture, NULL, &lifeRect);
      }
    }

    // H. HUD: Score
    if (ctx->font) {
      char scoreText[32];
      snprintf(scoreText, sizeof(scoreText), "SCORE: %05d", player->score);

      SDL_Color color = {255, 255, 255, 255}; // White
      SDL_Surface *surface =
          TTF_RenderText_Solid(ctx->font, scoreText, 0, color);

      if (surface) {
        SDL_Texture *scoreTexture =
            SDL_CreateTextureFromSurface(ctx->renderer, surface);

        if (scoreTexture) {
          // Align Top-Right
          SDL_FRect scoreRect = {(float)(LOGICAL_WIDTH - surface->w - 20),
                                 10.0f, (float)surface->w, (float)surface->h};
          SDL_RenderTexture(ctx->renderer, scoreTexture, NULL, &scoreRect);
          SDL_DestroyTexture(scoreTexture);
        }
        SDL_DestroySurface(surface);
      }
    }
  }

  // --- LAYER 2: STATE OVERLAYS ---
  // Renders semi-transparent backgrounds and text on top of the game

  SDL_Color white = {255, 255, 255, 255};
  SDL_Color yellow = {255, 255, 0, 255};
  SDL_Color red = {255, 0, 0, 255};
  SDL_Color green = {0, 255, 0, 255};

  if (gameState == STATE_MENU) {
    // Semi-transparent black overlay
    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 150);
    SDL_RenderFillRect(ctx->renderer, NULL);

    renderText(ctx, "SPACE INVADERS", 150, yellow);
    renderText(ctx, "Press ENTER to Start", 300, white);
    renderText(ctx, "Press ESC to Quit", 350, white);
  } else if (gameState == STATE_PAUSED) {
    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 100);
    SDL_RenderFillRect(ctx->renderer, NULL);

    renderText(ctx, "- PAUSED -", 250, yellow);
    renderText(ctx, "Press P to Resume", 320, white);
  } else if (gameState == STATE_GAME_OVER) {
    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 150);
    SDL_RenderFillRect(ctx->renderer, NULL);

    if (playerWon) {
      renderText(ctx, "MISSION ACCOMPLISHED!", 200, green);
      renderText(ctx, "YOU WIN", 250, green);
    } else {
      renderText(ctx, "MISSION FAILED", 200, red);
      renderText(ctx, "GAME OVER", 250, red);
    }

    renderText(ctx, "Press ENTER for Menu", 350, white);
  }

  // Present the final composed frame to the monitor
  SDL_RenderPresent(ctx->renderer);
}

void toggleFullscreen(SDL_Context *ctx) {
  if (!ctx || !ctx->window)
    return;
  Uint32 flags = SDL_GetWindowFlags(ctx->window);
  bool isFullscreen = flags & SDL_WINDOW_FULLSCREEN;
  SDL_SetWindowFullscreen(ctx->window, !isFullscreen);
}
