#include "../headers/lower_string.h"

char *lower_string(char *restrict string) {
  int i;
  for (i = 0; string[i] != '\0'; i++) {
    string[i] = (char)tolower((unsigned char)string[i]);
  }
  string[i] = '\0';
  return string;
}
