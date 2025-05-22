#include "cargo.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  char *file = NULL;
  cli_t *cli = cargo_new(argv[0], "Positional example");
  cli_pos_t P = {"FILE", "Input file path", &file, CARGO_REQUIRED};
  cargo_positional(cli, &P);
  if (cargo_parse(cli, argc, (char *const *)argv) != CARGO_ERROR_OK) {
    CARGO_GEN_HELP(cli);
    cargo_free(cli);
    return 1;
  }
  printf("file=%s\n", file);
  cargo_free(cli);
  return 0;
}
