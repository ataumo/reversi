#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "player.h"

static void rand_init(void) {
  static bool isinitialized = false;
  if (!isinitialized) {
    srandom(time(NULL) - getpid());
    isinitialized = true;
  }
}

int main() {
  /*char *args[] = {"./reversi", NULL};
  execvp(args[0], args);
  printf("Ending-----");

  char current_char = '\0';
  while (current_char != EOF) {
    current_char = fgetc(stdout);
  }*/
  int v = 0;
  int sum = 0;
  for (size_t i = 0; i < 10000; i++) {
    rand_init();
    v = (random() % 100 + 1);
    sum += v;
  }
  fprintf(stdout, "%d\n", sum / 10000);
  return 0;
}
