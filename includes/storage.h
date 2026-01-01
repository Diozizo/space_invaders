#ifndef STORAGE_H
#define STORAGE_H

/**
 * @brief Loads the highest score from the persistent storage (JSON file).
 * @return The high score, or 0 if no save file exists.
 */
int loadHighScore();

/**
 * @brief Saves the current score if it is higher than the existing record.
 * @param score The player's current score to check/save.
 */
void saveHighScore(int score);

#endif