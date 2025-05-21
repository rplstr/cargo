#include "cargo.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  int verbose = 0;
  cli_t *cli = cnew(argv[0], "Flag example: -v");
  cli_flag_t F = {'v', "verbose", "Enable verbose output", &verbose,
                  CARGO_OPTIONAL};
  cflag(cli, &F);
  if (cparse(cli, argc, (char *const *)argv) != CARGO_ERROR_OK) {
    CARGO_GEN_HELP(cli);
    cfree(cli);
    return 1;
  }
  printf("verbose=%d\n", verbose);
  cfree(cli);
  return 0;
}
