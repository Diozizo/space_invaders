/**
 * @file storage.c
 * @brief Manages persistent data storage for Space Invaders.
 *
 * This module handles the reading and writing of the High Score.
 * It uses Linux-specific logic to ensure the save file is always
 * stored in the same directory as the executable (the 'build' folder),
 * regardless of the current working directory.
 */

#define _POSIX_C_SOURCE 200809L
#include "../../includes/storage.h"
#include "../../includes/json_helper.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// ==========================================
//               CONSTANTS
// ==========================================
const char *SAVE_FILENAME = "savegame.json";
const char *KEY_NAME = "high_score";

// ==========================================
//            HELPER FUNCTIONS
// ==========================================

/**
 * @brief Constructs the absolute path to the save file.
 *
 * This function solves the problem of relative paths. If the user runs
 * "./build/spaceinvaders" or "cd build && ./spaceinvaders", the file
 * will always be created inside the 'build' directory.
 *
 * It works by reading the symbolic link '/proc/self/exe', which points
 * to the currently running binary on Linux systems.
 *
 * @param buffer The character buffer where the full path will be written.
 * @param size The maximum size of the buffer (usually PATH_MAX).
 */
static void getSaveFilePath(char *buffer, size_t size) {
  // 1. Read the path of the current executable
  // /proc/self/exe is a special file in Linux that represents the running
  // program.
  ssize_t len = readlink("/proc/self/exe", buffer, size - 1);

  if (len != -1) {
    buffer[len] = '\0'; // Null-terminate the string

    // Example buffer content: "/home/user/SpaceInvaders/build/spaceinvaders"

    // 2. Find the last slash '/' to separate directory from filename
    char *lastSlash = strrchr(buffer, '/');
    if (lastSlash) {
      // 3. Cut the string after the last slash
      // Becomes: "/home/user/SpaceInvaders/build/"
      *(lastSlash + 1) = '\0';
    }
  } else {
    // Fallback: If readlink fails, use an empty string (current directory)
    buffer[0] = '\0';
  }

  // 4. Append the save filename
  // Final Result: "/home/user/SpaceInvaders/build/savegame.json"
  strncat(buffer, SAVE_FILENAME, size - strlen(buffer) - 1);
}

// ==========================================
//           PUBLIC FUNCTIONS
// ==========================================

int loadHighScore() {
  char path[PATH_MAX];

  // Resolve the absolute path to build/savegame.json
  getSaveFilePath(path, sizeof(path));

  // Delegate parsing to the JSON helper
  int score = readIntFromJson(path, KEY_NAME);

  // Return 0 if file doesn't exist or key is missing
  if (score < 0)
    return 0;

  return score;
}

void saveHighScore(int newScore) {
  // 1. Check the current high score first
  int currentHigh = loadHighScore();

  // 2. Only perform a write operation if the new score is actually higher.
  // This reduces unnecessary file I/O operations.
  if (newScore > currentHigh) {
    char path[PATH_MAX];

    // Resolve path again to be safe
    getSaveFilePath(path, sizeof(path));

    // Write the new record to JSON
    writeIntToJson(path, KEY_NAME, newScore);

    // Optional: Debug log
    // printf("New High Score Saved to: %s\n", path);
  }
}