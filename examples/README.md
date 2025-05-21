# Examples

This directory contains standalone demos of each CLI feature provided by cargo.h.

## Flag Example

Demonstrates a boolean flag (`-v`/`--verbose`) that toggles verbose mode.

Build and run:
```sh
cc -std=c99 -Wall -I.. -o flag flag.c
./flag -v
# Output:
# verbose=1
``` 

## Option Example

Shows a string option (`-o`/`--output`) that accepts a value.

Build and run:
```sh
cc -std=c99 -Wall -I.. -o option option.c
./option -o result.txt
# Output:
# output=result.txt
``` 

## Enum Example

Illustrates an enumerated option (`-m`/`--mode`) with choices `A|B|C`.

Build and run:
```sh
cc -std=c99 -Wall -I.. -o enum enum.c
./enum --mode B
# Output:
# mode=B
``` 

## Positional Example

Covers a required positional argument (`FILE`) for input path.

Build and run:
```sh
cc -std=c99 -Wall -I.. -o positional positional.c
./positional data.txt
# Output:
# file=data.txt
```

## Subcommand Example

Demonstrates registering and invoking subcommands.

Build and run:
```sh
cc -std=c99 -Wall -I.. -o subcommand subcommand.c
./subcommand greet Alice
# Output:
# Hello, Alice!
./subcommand farewell Bob
# Output:
# Goodbye, Bob!
```
