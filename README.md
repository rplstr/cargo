# cargo

Single-header C library for simple CLI parsing with flags, options, enums, positional arguments, and subcommands.

## Usage

1. Copy `cargo.h` into your project.
2. Include in your source:
   ```c
   #include "cargo.h"
   ```
3. Create and configure parser:
   ```c
   cli_t *cli = cnew(argv[0], "epik app");
   // register flags:
   int verbose = 0;
   cli_flag_t F = {'v', "verbose", "Enable verbose output", &verbose, CARGO_OPTIONAL};
   cflag(cli, &F);
   // register options, enums, positionals, subcommands...
   ```
4. Parse arguments:
   ```c
   int rc = cparse(cli, argc, (char *const *)argv);
   if (rc != CARGO_ERROR_OK) {
       CARGO_GEN_HELP(cli);
       exit(rc);
   }
   ```
5. Use parsed values and free:
   ```c
   // use variables set by parser
   cfree(cli);
   ```


## License

This project is licensed under the 0BSD license. See [LICENSE](LICENSE) for full text.
