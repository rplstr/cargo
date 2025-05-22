#include "cargo.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  char *output = NULL;
  cli_t *cli = cargo_new(argv[0], "Option example: -o/--output");
  cli_opt_t O = {'o',     "output",      "FILE",        "Output file path",
                 &output, "default.txt", CARGO_OPTIONAL};
  cargo_option(cli, &O);
  if (cargo_parse(cli, argc, (char *const *)argv) != CARGO_ERROR_OK) {
    CARGO_GEN_HELP(cli);
    cargo_free(cli);
    return 1;
  }
  printf("output=%s\n", output);
  cargo_free(cli);
  return 0;
}
