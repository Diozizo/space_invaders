#ifndef SDL_VIEW_H
#define SDL_VIEW_H

#include "bunker.h"
#include "enemy.h"
#include "explosion.h"
#include "game_state.h"
#include "player.h"
#include "projectile.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

/**
 * @file sdl_view.h
 * @brief Manages the Graphical User Interface (GUI) using SDL3.
 *
 * This module is responsible for:
 * - Loading and storing assets (Textures, Fonts, Audio).
 * - Rendering game entities to the screen (Sprites).
 * - Playing sound effects and music.
 * - Managing the Application Window and Fullscreen toggling.
 */

// ==========================================
//               ENUMS
// ==========================================

/**
 * @brief Abstract identifiers for sound effects.
 * Used to decouple the game logic from specific SDL audio pointers.
 */
typedef enum {
  SOUND_PLAYER_SHOOT,    /**< Fired when player hits Space/A. */
  SOUND_ENEMY_SHOOT,     /**< Fired when an alien shoots. */
  SOUND_ENEMY_EXPLOSION, /**< Fired when an alien dies. */
  SOUND_PLAYER_EXPLOSION /**< Fired when the player dies. */
} SoundEffect;

// ==========================================
//               CONTEXT STRUCT
// ==========================================

/**
 * @brief The "God Struct" for the SDL View.
 * Holds references to the Window, Renderer, and all loaded assets.
 * This is passed to render functions so they have access to textures/sounds.
 */
typedef struct {
  // --- Core SDL ---
  SDL_Window *window;     /**< The OS window handle. */
  SDL_Renderer *renderer; /**< The GPU rendering context. */
  unsigned screenWidth;   /**< Logical screen width. */
  unsigned screenHeight;  /**< Logical screen height. */

  // --- Textures (Sprites) ---
  SDL_Texture *backgroundTexture;
  SDL_Texture *playerTexture;
  SDL_Texture *enemyTexture1; /**< Frame 1 of Enemy animation. */
  SDL_Texture *enemyTexture2; /**< Frame 2 of Enemy animation. */
  SDL_Texture *playerProjectileTexture;
  SDL_Texture *enemyProjectileTexture;
  SDL_Texture *bunkerTexture;
  SDL_Texture *bossTexture;

  /** @brief Array of frames for the explosion animation. */
  SDL_Texture *explosionTextures[3];

  /** @brief Array of frames for the player's engine exhaust (idle/moving). */
  SDL_Texture *playerExhaustTexture[4];

  /** @brief Array of frames for enemy exhaust/effects (if used). */
  SDL_Texture *exhaustTexture[4];

  // --- Typography ---
  TTF_Font *font; /**< Main font used for Score and Game Over text. */

  // --- Audio System (SDL3 Mixer) ---
  MIX_Mixer *mixer;           /**< Main Audio Mixer instance. */
  MIX_Audio *backgroundMusic; /**< Background loop (streamed). */
  MIX_Track *musicTrack;      /**< Handle for the playing track. */

  // --- Sound Effects (Pre-loaded memory chunks) ---
  MIX_Audio *sfxPlayerShoot;
  MIX_Audio *sfxEnemyShoot;
  MIX_Audio *sfxEnemyExplosion;
  MIX_Audio *sfxPlayerExplosion;

} SDL_Context;

// ==========================================
//               FUNCTIONS
// ==========================================

/**
 * @brief Initializes SDL, creates a Window and a Renderer, and loads assets.
 * * Performs the following steps:
 * 1. `SDL_Init` (Video, Audio, Events, Gamepad).
 * 2. Creates Window and Renderer.
 * 3. Initializes SDL_Image and loads all `.png` textures.
 * 4. Initializes SDL_Mixer and loads all `.wav` / `.mp3` files.
 * 5. Initializes SDL_TTF and loads the font.
 * * @param width  Logical width of the window.
 * @param height Logical height of the window.
 * @return SDL_Context* Pointer to the fully loaded context, or NULL on error.
 */
SDL_Context *initSDLView(unsigned width, unsigned height);

/**
 * @brief Cleans up all SDL resources.
 * Destroys textures, frees audio chunks, closes fonts, and shuts down SDL.
 * @param ctx Pointer to the context to destroy.
 */
void destroySDLView(SDL_Context *ctx);

/**
 * @brief Toggles between Windowed mode and Fullscreen Desktop mode.
 * @param ctx Pointer to the SDL Context.
 */
void toggleFullscreen(SDL_Context *ctx);

/**
 * @brief Plays a specific sound effect.
 * Maps the abstract `SoundEffect` enum to the actual `MIX_Audio` pointer
 * stored in the context and plays it on a free audio channel.
 * * @param ctx    Pointer to the SDL Context.
 * @param effect Enum identifying which sound to play.
 */
void playSound(SDL_Context *ctx, SoundEffect effect);

/**
 * @brief The Master Render Function.
 * * Clears the screen, draws the background, draws all game entities based on
 * the current `GameState` (Menu, Playing, Game Over), and presents the frame.
 * * @param ctx         Pointer to the SDL Context (holds textures).
 * @param player      Pointer to the Player model.
 * @param projectiles Pointer to the Projectile pool.
 * @param swarm       Pointer to the Enemy Swarm.
 * @param explosions  Pointer to the Explosion Manager.
 * @param bunkers     Pointer to the Bunker Manager.
 * @param gameState   Current state of the game (determines what screen to
 * draw).
 * @param playerWon   Boolean flag for the Game Over screen (Win vs Loss
 * message).
 */
void renderSDL(SDL_Context *ctx, const Player *player,
               const Projectiles *projectiles, const Swarm *swarm,
               const ExplosionManager *explosions, const BunkerManager *bunkers,
               GameState gameState, bool playerWon);

#endif // SDL_VIEW_H