#include "cargo.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  char *file = NULL;
  cli_t *cli = cnew(argv[0], "Positional example");
  cli_pos_t P = {"FILE", "Input file path", &file, CARGO_REQUIRED};
  cpos(cli, &P);
  if (cparse(cli, argc, (char *const *)argv) != CARGO_ERROR_OK) {
    CARGO_GEN_HELP(cli);
    cfree(cli);
    return 1;
  }
  printf("file=%s\n", file);
  cfree(cli);
  return 0;
}
