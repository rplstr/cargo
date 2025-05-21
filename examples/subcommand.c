#include <stdio.h>
#include <stdlib.h>
#include "../cargo.h"

int greet_handler(cli_t *subcli, int argc, char *const argv[]) {
    char *name = NULL;
    cli_pos_t P = {"NAME", "Name to greet", &name, CARGO_REQUIRED};
    cpos(subcli, &P);
    if (cparse(subcli, argc, argv) != CARGO_ERROR_OK) {
        CARGO_GEN_HELP(subcli);
        return 1;
    }
    printf("Hello, %s!\n", name);
    return 0;
}

int farewell_handler(cli_t *subcli, int argc, char *const argv[]) {
    char *name = NULL;
    cli_pos_t P = {"NAME", "Name to farewell", &name, CARGO_REQUIRED};
    cpos(subcli, &P);
    if (cparse(subcli, argc, argv) != CARGO_ERROR_OK) {
        CARGO_GEN_HELP(subcli);
        return 1;
    }
    printf("Goodbye, %s!\n", name);
    return 0;
}

int main(int argc, char **argv) {
    cli_t *cli = cnew(argv[0], "Subcommand example");
    cli_t *greet_cli = cnew("greet", "Greet someone");
    cli_t *farewell_cli = cnew("farewell", "Farewell someone");

    cli_cmd_t greet_cmd = {"greet", "Print greeting", greet_cli, greet_handler};
    cli_cmd_t farewell_cmd = {"farewell", "Print farewell", farewell_cli, farewell_handler};
    ccmd(cli, &greet_cmd);
    ccmd(cli, &farewell_cmd);

    if (cparse(cli, argc, (char *const *)argv) != CARGO_ERROR_OK) {
        CARGO_GEN_HELP(cli);
        cfree(cli);
        return 1;
    }

    cfree(greet_cli);
    cfree(farewell_cli);
    cfree(cli);
    return 0;
}
