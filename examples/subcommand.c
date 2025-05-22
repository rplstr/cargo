#include "../cargo.h"
#include <stdio.h>
#include <stdlib.h>

int greet_handler(cli_t *subcli, int argc, char *const argv[]) {
  char *name = NULL;
  cli_pos_t P = {"NAME", "Name to greet", &name, CARGO_REQUIRED};
  cargo_positional(subcli, &P);
  if (cargo_parse(subcli, argc, argv) != CARGO_ERROR_OK) {
    CARGO_GEN_HELP(subcli);
    return 1;
  }
  printf("Hello, %s!\n", name);
  return 0;
}

int farewell_handler(cli_t *subcli, int argc, char *const argv[]) {
  char *name = NULL;
  cli_pos_t P = {"NAME", "Name to farewell", &name, CARGO_REQUIRED};
  cargo_positional(subcli, &P);
  if (cargo_parse(subcli, argc, argv) != CARGO_ERROR_OK) {
    CARGO_GEN_HELP(subcli);
    return 1;
  }
  printf("Goodbye, %s!\n", name);
  return 0;
}

int main(int argc, char **argv) {
  cli_t *cli = cargo_new(argv[0], "Subcommand example");
  cli_t *greet_cli = cargo_new("greet", "Greet someone");
  cli_t *farewell_cli = cargo_new("farewell", "Farewell someone");

  cli_cmd_t greet_cmd = {"greet", "Print greeting", greet_cli, greet_handler};
  cli_cmd_t farewell_cmd = {"farewell", "Print farewell", farewell_cli,
                            farewell_handler};
  cargo_subcommand(cli, &greet_cmd);
  cargo_subcommand(cli, &farewell_cmd);

  if (cargo_parse(cli, argc, (char *const *)argv) != CARGO_ERROR_OK) {
    CARGO_GEN_HELP(cli);
    cargo_free(cli);
    return 1;
  }

  cargo_free(greet_cli);
  cargo_free(farewell_cli);
  cargo_free(cli);
  return 0;
}
