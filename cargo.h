/* SPDX-License-Identifier: 0BSD */
/**
 * cargo.h — simple single-header CLI parsing library for C.
 */
#ifndef CARGO_H_
#define CARGO_H_

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLI_MAX_FLAGS 32
#define CLI_MAX_OPTS 32
#define CLI_MAX_ENUMS 32
#define CLI_MAX_POS 32
#define CLI_MAX_CMDS 16

#ifndef CARGO_ALLOC
#define CARGO_ALLOC malloc
#endif
#ifndef CARGO_FREE
#define CARGO_FREE free
#endif
#ifndef CARGO_GEN_HELP
#define CARGO_GEN_HELP cargo_help
#endif

/* requirement: optional or required */
#ifndef CARGO_OPTIONAL
#define CARGO_OPTIONAL 0
#endif
#ifndef CARGO_REQUIRED
#define CARGO_REQUIRED 1
#endif

/* error codes */
#ifndef CARGO_ERROR_OK
#define CARGO_ERROR_OK 0
#endif
#ifndef CARGO_ERROR_REQUIRED
#define CARGO_ERROR_REQUIRED 1
#endif
#ifndef CARGO_ERROR_MISSING_VALUE
#define CARGO_ERROR_MISSING_VALUE 2
#endif
#ifndef CARGO_ERROR_UNRECOGNIZED_OPTION
#define CARGO_ERROR_UNRECOGNIZED_OPTION 3
#endif

#ifndef CARGO_PERROR
#define CARGO_PERROR(...) cargo_perror(__VA_ARGS__)
#endif

#ifndef CARGO_NAME_MAX
#define CARGO_NAME_MAX 64
#endif

/**
 * cli_t — opaque CLI parser handle
 */
typedef struct cli cli_t;

/**
 * cli_flag_t — boolean flag config
 * @s:    short option char (e.g. 'v')
 * @name: long option name (e.g. "verbose")
 * @help: help text
 * @dest: pointer to int to store 0 or 1
 * @req:  requirement (CARGO_OPTIONAL or CARGO_REQUIRED)
 */
typedef struct {
  char s;
  const char *name;
  const char *help;
  int *dest;
  int req;
} cli_flag_t;

/**
 * cli_opt_t — string option config
 * @s:    short option char
 * @name: long option name
 * @arg:  meta-variable name
 * @help: help text
 * @dest: pointer to char* to store result
 * @def:  default value
 * @req:  requirement (CARGO_OPTIONAL or CARGO_REQUIRED)
 */
typedef struct {
  char s;
  const char *name;
  const char *arg;
  const char *help;
  char **dest;
  const char *def;
  int req;
} cli_opt_t;

/**
 * cli_enum_t — enumerated option config
 * @s:        short option char
 * @name:     long option name
 * @arg:      meta-variable name
 * @help:     help text
 * @dest:     pointer to int to store index
 * @choices:  array of choice strings
 * @nchoices: number of choices
 * @def:      default index
 * @req:      requirement (CARGO_OPTIONAL or CARGO_REQUIRED)
 */
typedef struct {
  char s;
  const char *name;
  const char *arg;
  const char *help;
  int *dest;
  const char *const *choices;
  size_t nchoices;
  int def;
  int req;
} cli_enum_t;

/**
 * cli_pos_t — positional argument config
 * @name: meta-variable name
 * @help: help text
 * @dest: pointer to char* to store result
 * @req:  requirement (CARGO_OPTIONAL or CARGO_REQUIRED)
 */
typedef struct {
  const char *name;
  const char *help;
  char **dest;
  int req;
} cli_pos_t;

typedef int (*cli_cmd_handler_t)(cli_t *, int, char *const[]);

typedef struct {
  const char *name;
  const char *help;
  cli_t *sub;
  cli_cmd_handler_t handler;
} cli_cmd_t;

struct cli {
  const char *prog;
  const char *desc;
  size_t nflag;
  const cli_flag_t *flags[CLI_MAX_FLAGS];
  size_t nopt;
  const cli_opt_t *opts[CLI_MAX_OPTS];
  size_t nenm;
  const cli_enum_t *enums[CLI_MAX_ENUMS];
  size_t npos;
  const cli_pos_t *poss[CLI_MAX_POS];
  size_t ncmd;
  const cli_cmd_t *cmds[CLI_MAX_CMDS];
};

/**
 * cargo_new — allocate a new CLI parser
 * @prog: program name (argv[0])
 * @desc: description for help
 * returns: cli_t* or NULL on failure
 */
static inline cli_t *cargo_new(const char *prog, const char *desc) {
  cli_t *c = CARGO_ALLOC(sizeof(*c));
  if (!c)
    return NULL;
  c->prog = prog;
  c->desc = desc;
  c->nflag = c->nopt = c->nenm = c->npos = c->ncmd = 0;
  return c;
}

/* default error printer */
static inline void cargo_perror(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "cargo: \033[1;31merror\033[0m: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

/**
 * cargo_free — free parser
 * @c: parser handle
 */
static inline void cargo_free(cli_t *c) { CARGO_FREE(c); }

/**
 * cargo_flag — register a boolean flag
 * returns: 0 on success, -1 on error
 */
static inline int cargo_flag(cli_t *c, const cli_flag_t *f) {
  if (!c || !f || !f->dest || c->nflag >= CLI_MAX_FLAGS)
    return -1;
  /* uniqueness of flag char/name */
  for (size_t j = 0; j < c->nflag; j++) {
    if (c->flags[j]->s == f->s || !strcmp(c->flags[j]->name, f->name))
      return -1;
  }
  /* avoid conflict with options */
  for (size_t j = 0; j < c->nopt; j++) {
    if (c->opts[j]->s == f->s || !strcmp(c->opts[j]->name, f->name))
      return -1;
  }
  /* avoid conflict with enums */
  for (size_t j = 0; j < c->nenm; j++) {
    if (c->enums[j]->s == f->s || !strcmp(c->enums[j]->name, f->name))
      return -1;
  }
  c->flags[c->nflag++] = f;
  *f->dest = 0;
  return 0;
}

/**
 * cargo_option — register a string option
 */
static inline int cargo_option(cli_t *c, const cli_opt_t *o) {
  if (!c || !o || !o->dest || c->nopt >= CLI_MAX_OPTS)
    return -1;
  /* uniqueness of option char/name */
  for (size_t j = 0; j < c->nopt; j++) {
    if (c->opts[j]->s == o->s || !strcmp(c->opts[j]->name, o->name))
      return -1;
  }
  for (size_t j = 0; j < c->nflag; j++) {
    if (c->flags[j]->s == o->s || !strcmp(c->flags[j]->name, o->name))
      return -1;
  }
  for (size_t j = 0; j < c->nenm; j++) {
    if (c->enums[j]->s == o->s || !strcmp(c->enums[j]->name, o->name))
      return -1;
  }
  c->opts[c->nopt++] = o;
  *o->dest = (char *)o->def;
  return 0;
}

/**
 * cargo_enum — register an enumerated option
 */
static inline int cargo_enum(cli_t *c, const cli_enum_t *e) {
  if (!c || !e || !e->dest || !e->choices || c->nenm >= CLI_MAX_ENUMS)
    return -1;
  /* uniqueness of enum char/name */
  for (size_t j = 0; j < c->nenm; j++) {
    if (c->enums[j]->s == e->s || !strcmp(c->enums[j]->name, e->name))
      return -1;
  }
  for (size_t j = 0; j < c->nflag; j++) {
    if (c->flags[j]->s == e->s || !strcmp(c->flags[j]->name, e->name))
      return -1;
  }
  for (size_t j = 0; j < c->nopt; j++) {
    if (c->opts[j]->s == e->s || !strcmp(c->opts[j]->name, e->name))
      return -1;
  }
  c->enums[c->nenm++] = e;
  *e->dest = e->def;
  return 0;
}

/**
 * cargo_positional — register a positional argument
 */
static inline int cargo_positional(cli_t *c, const cli_pos_t *p) {
  if (!c || !p || !p->dest || c->npos >= CLI_MAX_POS)
    return -1;
  /* uniqueness of positional name */
  for (size_t j = 0; j < c->npos; j++) {
    if (!strcmp(c->poss[j]->name, p->name))
      return -1;
  }
  c->poss[c->npos++] = p;
  *p->dest = NULL;
  return 0;
}

/**
 * cargo_subcommand — register a subcommand
 */
static inline int cargo_subcommand(cli_t *c, const cli_cmd_t *cmd) {
  if (!c || !cmd || !cmd->name || c->ncmd >= CLI_MAX_CMDS)
    return -1;
  for (size_t j = 0; j < c->ncmd; j++)
    if (!strcmp(c->cmds[j]->name, cmd->name))
      return -1;
  c->cmds[c->ncmd++] = cmd;
  return 0;
}

/**
 * cargo_help — print usage/help
 */
static inline void cargo_help(const cli_t *c) {
  size_t i, j;
  printf("usage: %s", c->prog);
  if (c->ncmd)
    printf(" <command>");
  if (c->nflag || c->nopt || c->nenm)
    printf(" [options]");
  for (i = 0; i < c->npos; i++)
    printf(" <%s>", c->poss[i]->name);
  printf("\n\n%s\n", c->desc);
  if (c->ncmd) {
    printf("\ncommands:\n");
    for (i = 0; i < c->ncmd; i++)
      printf("  %-10s %s\n", c->cmds[i]->name, c->cmds[i]->help);
  }
  printf("\noptions:\n");
  for (i = 0; i < c->nflag; i++) {
    const cli_flag_t *f = c->flags[i];
    printf("  -%c, --%-10s %s\n", f->s, f->name, f->help);
  }
  for (i = 0; i < c->nopt; i++) {
    const cli_opt_t *o = c->opts[i];
    printf("  -%c, --%-10s <%s> %s (default: %s)\n", o->s, o->name, o->arg,
           o->help, o->def);
  }
  for (i = 0; i < c->nenm; i++) {
    const cli_enum_t *e = c->enums[i];
    printf("  -%c, --%-10s <%s> %s [", e->s, e->name, e->arg, e->help);
    for (j = 0; j < e->nchoices; j++)
      printf(j ? "|%s" : "%s", e->choices[j]);
    printf("] (default: %s)\n", e->choices[e->def]);
  }
  for (i = 0; i < c->npos; i++) {
    const cli_pos_t *p = c->poss[i];
    printf("     %-13s %s\n", p->name, p->help);
  }
}

/* cargo_parse helpers */
static inline int handle_named(cli_t *c, const char *name, const char *val) {
  /* boolean flags */
  for (size_t j = 0; j < c->nflag; j++) {
    if (!strcmp(name, c->flags[j]->name)) {
      if (val) {
        CARGO_PERROR("flag --%s does not take a value", name);
        return CARGO_ERROR_UNRECOGNIZED_OPTION;
      }
      *c->flags[j]->dest = 1;
      return CARGO_ERROR_OK;
    }
  }
  /* string options */
  for (size_t j = 0; j < c->nopt; j++) {
    if (!strcmp(name, c->opts[j]->name)) {
      if (!val) {
        CARGO_PERROR("missing value for option: --%s", name);
        return CARGO_ERROR_MISSING_VALUE;
      }
      *c->opts[j]->dest = (char *)val;
      return CARGO_ERROR_OK;
    }
  }
  /* enum options */
  for (size_t j = 0; j < c->nenm; j++) {
    if (!strcmp(name, c->enums[j]->name)) {
      if (!val) {
        CARGO_PERROR("missing value for option: --%s", name);
        return CARGO_ERROR_MISSING_VALUE;
      }
      for (size_t k = 0; k < c->enums[j]->nchoices; k++) {
        if (!strcmp(val, c->enums[j]->choices[k])) {
          *c->enums[j]->dest = (int)k;
          return CARGO_ERROR_OK;
        }
      }
      CARGO_PERROR("invalid value '%s' for option --%s", val, name);
      return CARGO_ERROR_UNRECOGNIZED_OPTION;
    }
  }
  /* unknown option */
  CARGO_PERROR("unrecognized option: --%s", name);
  return CARGO_ERROR_UNRECOGNIZED_OPTION;
}

static inline int handle_shorthand(cli_t *c, char s, const char *val) {
  /* boolean flags */
  for (size_t j = 0; j < c->nflag; j++) {
    if (c->flags[j]->s == s) {
      if (val) {
        CARGO_PERROR("flag -%c does not take a value", s);
        return CARGO_ERROR_UNRECOGNIZED_OPTION;
      }
      *c->flags[j]->dest = 1;
      return CARGO_ERROR_OK;
    }
  }
  /* string options */
  for (size_t j = 0; j < c->nopt; j++) {
    if (c->opts[j]->s == s) {
      if (!val) {
        CARGO_PERROR("missing value for option: -%c/--%s", s, c->opts[j]->name);
        return CARGO_ERROR_MISSING_VALUE;
      }
      *c->opts[j]->dest = (char *)val;
      return CARGO_ERROR_OK;
    }
  }
  /* enum options */
  for (size_t j = 0; j < c->nenm; j++) {
    if (c->enums[j]->s == s) {
      if (!val) {
        CARGO_PERROR("missing value for option: -%c/--%s", s,
                     c->enums[j]->name);
        return CARGO_ERROR_MISSING_VALUE;
      }
      for (size_t m = 0; m < c->enums[j]->nchoices; m++) {
        if (!strcmp(val, c->enums[j]->choices[m])) {
          *c->enums[j]->dest = (int)m;
          return CARGO_ERROR_OK;
        }
      }
      CARGO_PERROR("invalid value '%s' for option: -%c/--%s", val, s,
                   c->enums[j]->name);
      return CARGO_ERROR_UNRECOGNIZED_OPTION;
    }
  }
  /* unknown short option */
  CARGO_PERROR("unrecognized option: -%c", s);
  return CARGO_ERROR_UNRECOGNIZED_OPTION;
}

static inline int parse_long(cli_t *c, char *arg, size_t *idx, int argc,
                             char *const argv[]) {
  char *eq = strchr(arg + 2, '=');
  size_t L = eq ? (size_t)(eq - (arg + 2)) : strlen(arg + 2);
  char name[CARGO_NAME_MAX];
  if (L >= CARGO_NAME_MAX) {
    CARGO_PERROR("unrecognized option: %s", arg);
    return CARGO_ERROR_UNRECOGNIZED_OPTION;
  }
  memcpy(name, arg + 2, L);
  name[L] = '\0';
  if (eq) {
    /* attached value */
    return handle_named(c, name, eq + 1);
  }
  /* no '=', flag? */
  for (size_t j = 0; j < c->nflag; j++) {
    if (!strcmp(name, c->flags[j]->name))
      return handle_named(c, name, NULL);
  }
  /* option or enum: consume next arg */
  const char *val = NULL;
  if (++*idx < (size_t)argc)
    val = argv[*idx];
  return handle_named(c, name, val);
}

static inline int parse_short(cli_t *c, char *arg, size_t *idx, int argc,
                              char *const argv[]) {
  for (size_t k = 1; arg[k]; k++) {
    char s = arg[k];
    /* flag? */
    int is_flag = 0;
    for (size_t j = 0; j < c->nflag; j++) {
      if (c->flags[j]->s == s) {
        is_flag = 1;
        int rc = handle_shorthand(c, s, NULL);
        if (rc != CARGO_ERROR_OK)
          return rc;
        break;
      }
    }
    if (is_flag)
      continue;
    /* option or enum: attached or next */
    const char *val = NULL;
    if (arg[k + 1]) {
      val = arg + k + 1;
    } else if (++*idx < (size_t)argc) {
      val = argv[*idx];
    }
    return handle_shorthand(c, s, val);
  }
  return CARGO_ERROR_OK;
}

/**
 * cargo_parse — parse argc/argv with configurable alloc/free/help
 */
static inline int cargo_parse(cli_t *c, int argc, char *const argv[]) {
  size_t i, pos = 0;
  int rc;
  for (i = 1; i < (size_t)argc; i++) {
    char *arg = argv[i];
    if (!strcmp(arg, "--")) {
      while (++i < (size_t)argc && pos < c->npos)
        *c->poss[pos++]->dest = argv[i];
      break;
    }
    if (!strcmp(arg, "-h") || !strcmp(arg, "--help")) {
      CARGO_GEN_HELP(c);
      return CARGO_ERROR_OK;
    }
    /* subcommands */
    if (c->ncmd) {
      for (size_t j = 0; j < c->ncmd; j++) {
        const cli_cmd_t *sc = c->cmds[j];
        if (!strcmp(arg, sc->name)) {
          if (sc->handler)
            return sc->handler(sc->sub, (int)(argc - i), argv + i);
          CARGO_GEN_HELP(sc->sub);
          return CARGO_ERROR_OK;
        }
      }
    }
    if (arg[0] == '-' && arg[1] == '-') {
      if ((rc = parse_long(c, arg, &i, argc, argv)) != CARGO_ERROR_OK)
        return rc;
      continue;
    }
    if (arg[0] == '-' && arg[1]) {
      if ((rc = parse_short(c, arg, &i, argc, argv)) != CARGO_ERROR_OK)
        return rc;
      continue;
    }
    if (pos >= c->npos) {
      CARGO_PERROR("unrecognized option: %s", arg);
      return CARGO_ERROR_UNRECOGNIZED_OPTION;
    }
    *c->poss[pos++]->dest = arg;
  }
  /* check required arguments */
  for (size_t j = 0; j < c->nflag; j++) {
    if (c->flags[j]->req == CARGO_REQUIRED && *c->flags[j]->dest == 0) {
      CARGO_PERROR("missing required flag: --%s", c->flags[j]->name);
      return CARGO_ERROR_REQUIRED;
    }
  }
  for (size_t j = 0; j < c->nopt; j++) {
    if (c->opts[j]->req == CARGO_REQUIRED && *c->opts[j]->dest == NULL) {
      CARGO_PERROR("missing required option: --%s", c->opts[j]->name);
      return CARGO_ERROR_REQUIRED;
    }
  }
  for (size_t j = 0; j < c->nenm; j++) {
    if (c->enums[j]->req == CARGO_REQUIRED && *c->enums[j]->dest < 0) {
      CARGO_PERROR("missing required option: --%s", c->enums[j]->name);
      return CARGO_ERROR_REQUIRED;
    }
  }
  for (size_t j = 0; j < c->npos; j++) {
    if (c->poss[j]->req == CARGO_REQUIRED && *c->poss[j]->dest == NULL) {
      CARGO_PERROR("missing required argument: %s", c->poss[j]->name);
      return CARGO_ERROR_REQUIRED;
    }
  }
  return CARGO_ERROR_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* CARGO_H_ */
