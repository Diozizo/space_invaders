#ifndef JSON_HELPER_H
#define JSON_HELPER_H

/**
 * @brief Reads a specific integer key from a simple JSON file.
 * Returns -1 if key not found or file error.
 */
int readIntFromJson(const char *filename, const char *key);

/**
 * @brief Writes a single key-value pair to a JSON file.
 * Example: { "high_score": 1000 }
 */
void writeIntToJson(const char *filename, const char *key, int value);

#endif