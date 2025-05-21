#include "cargo.h"
#include <stdio.h>
#include <stdlib.h>

enum { MODE_A, MODE_B, MODE_C };

int main(int argc, char **argv) {
  int mode = MODE_A;
  const char *choices[] = {"A", "B", "C"};
  cli_t *cli = cnew(argv[0], "Enum example: -m/--mode");
  cli_enum_t E = {'m',     "mode", "MODE", "Select mode", &mode,
                  choices, 3,      MODE_A, CARGO_OPTIONAL};
  cenum(cli, &E);
  if (cparse(cli, argc, (char *const *)argv) != CARGO_ERROR_OK) {
    CARGO_GEN_HELP(cli);
    cfree(cli);
    return 1;
  }
  printf("mode=%s\n", choices[mode]);
  cfree(cli);
  return 0;
}
