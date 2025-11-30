#include "../headers/leading_whitespace.h"
#include <stdio.h>
#include <string.h>

char *leading_whitespace(char *word_buff) {
  int word_count = 0;

  while (word_buff[word_count] == ' ') {
    int word_len = strlen(word_buff);

    memmove(&word_buff[word_count], &word_buff[word_count + 1], word_len);
  }

  return word_buff;
}
