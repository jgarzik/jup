
#include "jup-config.h"
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <argp.h>
#include <univalue.h>

using namespace std;

#define PROGRAM_NAME "jup"

const char *argp_program_version =
PROGRAM_NAME " " VERSION;
static const char doc[] =
PROGRAM_NAME " - JSON swiss army knife";
static const char args_doc[] =
"EDIT-COMMANDS...";

static struct argp_option options[] = {
	{"min", 1001, 0, 0, "Minimize JSON output"},
	{ }
};

static error_t parse_opt (int key, char *arg, struct argp_state *state);
static const struct argp argp = { options, parse_opt, args_doc, doc };

static bool readStdin = true;
static bool minimalJson = false;
UniValue jdoc(UniValue::VNULL);

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	switch (key) {
	case 1001:
		minimalJson = true;
		break;

	case ARGP_KEY_END:
		break;

	default:
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

static const int INPUT_BUFSIZE = 8192;

static bool readStringFd(int fd, string& rawBody)
{
	do {
		vector<unsigned char> buf(INPUT_BUFSIZE);

		ssize_t rrc = read(fd, &buf[0], buf.size());
		if (rrc < 0) {
			perror("(stdin)");
			return false;
		}
		if (rrc == 0)
			break;

		rawBody.append((char *) &buf[0], rrc);
	} while (1);

	return true;
}

static bool writeStringFd(int fd, const string& rawBody)
{
	ssize_t written = 0;
	ssize_t to_write = rawBody.size();
	do {
		ssize_t wrc = write(fd, &rawBody[written], to_write);
		if (wrc < 0) {
			perror("(stdin)");
			return false;
		}

		written += wrc;
		to_write -= wrc;
	} while (to_write > 0);

	return true;
}

static bool readInput()
{
	string rawBody;

	if (!readStringFd(STDIN_FILENO, rawBody))
		return false;

	if (!jdoc.read(rawBody)) {
		fprintf(stderr, "(stdin): Invalid JSON input\n");
		return false;
	}

	return true;
}

static bool processDocument()
{
	return true;
}

static bool writeOutput()
{
	string rawBody = jdoc.write(minimalJson ? 0 : 2);
	rawBody.append("\n");

	return writeStringFd(STDOUT_FILENO, rawBody);
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

	if ((readStdin && !readInput()) ||
	    !processDocument() ||
	    !writeOutput())
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

