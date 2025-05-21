#include "cargo.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  char *output = NULL;
  cli_t *cli = cnew(argv[0], "Option example: -o/--output");
  cli_opt_t O = {'o',     "output",      "FILE",        "Output file path",
                 &output, "default.txt", CARGO_OPTIONAL};
  copt(cli, &O);
  if (cparse(cli, argc, (char *const *)argv) != CARGO_ERROR_OK) {
    CARGO_GEN_HELP(cli);
    cfree(cli);
    return 1;
  }
  printf("output=%s\n", output);
  cfree(cli);
  return 0;
}
