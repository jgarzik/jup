
#include "jup-config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>

#define PROGRAM_NAME "jup"

const char *argp_program_version =
PROGRAM_NAME " " VERSION;
static const char doc[] =
PROGRAM_NAME " - JSON swiss army knife";
static const char args_doc[] =
"EDIT-COMMANDS...";

static struct argp_option options[] = {
	{ }
};

static error_t parse_opt (int key, char *arg, struct argp_state *state);
static const struct argp argp = { options, parse_opt, args_doc, doc };

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	switch (key) {

	case ARGP_KEY_END:
		break;

	default:
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

int main (int argc, char *argv[])
{
	// parse command line
	error_t argp_rc = argp_parse(&argp, argc, argv, 0, NULL, NULL);
	if (argp_rc) {
		fprintf(stderr, "%s: argp_parse failed: %s\n",
			argv[0], strerror(argp_rc));
		return EXIT_FAILURE;
	}

	return 0;
}

