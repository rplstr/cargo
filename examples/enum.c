#include "cargo.h"
#include <stdio.h>
#include <stdlib.h>

enum { MODE_A, MODE_B, MODE_C };

int main(int argc, char **argv) {
  int mode = MODE_A;
  const char *choices[] = {"A", "B", "C"};
  cli_t *cli = cargo_new(argv[0], "Enum example: -m/--mode");
  cli_enum_t E = {'m',     "mode", "MODE", "Select mode", &mode,
                  choices, 3,      MODE_A, CARGO_OPTIONAL};
  cargo_enum(cli, &E);
  if (cargo_parse(cli, argc, (char *const *)argv) != CARGO_ERROR_OK) {
    CARGO_GEN_HELP(cli);
    cargo_free(cli);
    return 1;
  }
  printf("mode=%s\n", choices[mode]);
  cargo_free(cli);
  return 0;
}
