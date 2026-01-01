#include "../../includes/json_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int readIntFromJson(const char *filename, const char *key) {
  FILE *f = fopen(filename, "r");
  if (!f)
    return 0; // File doesn't exist yet, return 0 score

  // Read entire file into buffer (assume small file < 1KB)
  char buffer[1024];
  size_t len = fread(buffer, 1, sizeof(buffer) - 1, f);
  buffer[len] = '\0';
  fclose(f);

  // Look for key (e.g., "high_score")
  char searchKey[128];
  snprintf(searchKey, sizeof(searchKey), "\"%s\"", key);

  char *found = strstr(buffer, searchKey);
  if (!found)
    return 0;

  // Move pointer past the key
  found += strlen(searchKey);

  // Look for colon ':'
  found = strchr(found, ':');
  if (!found)
    return 0;

  // Move past colon and Parse int
  return atoi(found + 1);
}

void writeIntToJson(const char *filename, const char *key, int value) {
  FILE *f = fopen(filename, "w");
  if (!f)
    return;

  // Write valid JSON format
  fprintf(f, "{\n  \"%s\": %d\n}\n", key, value);

  fclose(f);
}