#include "cargo.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  int verbose = 0;
  cli_t *cli = cargo_new(argv[0], "Flag example: -v");
  cli_flag_t F = {'v', "verbose", "Enable verbose output", &verbose,
                  CARGO_OPTIONAL};
  cargo_flag(cli, &F);
  if (cargo_parse(cli, argc, (char *const *)argv) != CARGO_ERROR_OK) {
    CARGO_GEN_HELP(cli);
    cargo_free(cli);
    return 1;
  }
  printf("verbose=%d\n", verbose);
  cargo_free(cli);
  return 0;
}
