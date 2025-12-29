#include "../../includes/ncurses_controller.h"
#include <ncurses.h>

#define NCURSES_SPEED 400.0f // Faster speed for terminal feel
#define GAME_WIDTH 800

bool handleNcursesInput(Player *player, Projectiles *bullets, GameState *state,
                        bool *needsReset, float deltaTime) {
  int ch = getch();

  if (ch == ERR)
    return true; // No input this frame

  switch (ch) {
  case 'q':
    return false; // Quit game

  // --- MENU / GAME OVER ---
  case '\n':
    if (*state == STATE_MENU)
      *state = STATE_PLAYING;
    if (*state == STATE_GAME_OVER) {
      *state = STATE_MENU;
      *needsReset = true;
    }
    break;

  // --- PAUSE ---
  case 'p':
    if (*state == STATE_PLAYING)
      *state = STATE_PAUSED;
    else if (*state == STATE_PAUSED)
      *state = STATE_PLAYING;
    break;

  // --- GAMEPLAY (Only works if Playing) ---
  case ' ':
    if (*state == STATE_PLAYING) {
      playerShoot(player, bullets);
    }
    break;

  case 'a':
  case KEY_LEFT:
    if (*state == STATE_PLAYING) {
      player->x -= NCURSES_SPEED * deltaTime;
      if (player->x < 0)
        player->x = 0;
    }
    break;

  case 'd':
  case KEY_RIGHT:
    if (*state == STATE_PLAYING) {
      player->x += NCURSES_SPEED * deltaTime;
      if (player->x > GAME_WIDTH - player->width)
        player->x = GAME_WIDTH - player->width;
    }
    break;
  }
  return true;
}