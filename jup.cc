
#include "jup-config.h"
#include <vector>
#include <deque>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
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
	{"list-commands", 1002, 0, 0, "List all supported edit commands"},

	{ }
};

class commandInfo {
public:
	unsigned int n_args;
	string name;
	string summary;
	string description;
	bool ignoreStdin;
};

static commandInfo commandList[] = {
	{ 1, "get", "get JSON-PATH",
	  "Replace document with subset of JSON input, starting at JSON-PATH" },
};

static error_t parse_opt (int key, char *arg, struct argp_state *state);
static const struct argp argp = { options, parse_opt, args_doc, doc };

static bool readStdin = true;
static bool minimalJson = false;
static bool doListCommands = false;
UniValue jdoc(UniValue::VNULL);
map<string,commandInfo> cmdMap;
deque<string> inputTokens;

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif

static void staticInit()
{
	for (unsigned int i = 0; i < ARRAY_SIZE(commandList); i++) {
		cmdMap[commandList[i].name] = commandList[i];
	}
}

static void listCommands()
{
	UniValue l(UniValue::VARR);

	for (map<string,commandInfo>::const_iterator it = cmdMap.begin();
	     it != cmdMap.end(); it++) {
		UniValue obj(UniValue::VOBJ);

		const commandInfo& ci = it->second;

		obj.pushKV("command", ci.name);
		obj.pushKV("usage", ci.summary);
		obj.pushKV("help", ci.description);

		l.push_back(obj);
	}

	printf("%s\n", l.write(minimalJson ? 0 : 2).c_str());
}

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	switch (key) {
	case 1001:
		minimalJson = true;
		break;

	case 1002:
		doListCommands = true;
		break;

	case ARGP_KEY_ARG:
		inputTokens.push_back(arg);
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

static void strsplit(const std::string& s_in, const std::string& delim,
		     std::deque<std::string>& v)
{
	char s[s_in.size() + 1];
	strcpy(s, s_in.c_str());

	char *p = strtok(s, delim.c_str());
	while (p != NULL) {
		v.push_back(p);
		p = strtok(NULL, delim.c_str());
	}
}

static bool isDigitStr(const string& s)
{
	for (unsigned int i = 0; i < s.size(); i++)
		if (!isdigit(s[i]))
			return false;

	return true;
}

static const UniValue& lookupPath(const string& path, deque<string> rem)
{
	rem.clear();

	deque<string> tokens;

	strsplit(path, ".", tokens);

	UniValue& jptr = jdoc;
	while (tokens.size() > 0) {
		const string& token = tokens.front();

		if (jptr.isObject() && jptr.exists(token)) {
			// direct path match
			if (tokens.size() == 1)
				return jptr[token];

			if (!jptr[token].isObject() &&
			    !jptr[token].isArray()) {
				rem = tokens;
				return jptr;
			}

			jptr = jptr[token];
			tokens.pop_front();

		} else if (jptr.isArray() && isDigitStr(token) &&
			   (atol(token.c_str()) < jptr.size())) {

			size_t index = atol(token.c_str());
			// direct path match
			if (tokens.size() == 1)
				return jptr[index];

			if (!jptr[index].isObject() &&
			    !jptr[index].isArray()) {
				rem = tokens;
				return jptr;
			}

			jptr = jptr[index];
			tokens.pop_front();
		} else {
			rem = tokens;
			return jptr;
		}
	}

	return jptr;
}

const UniValue& objectGet(const string& jpath)
{
	deque<string> rem;

	const UniValue& val = lookupPath(jpath, rem);
	if (rem.size() != 0)
		return NullUniValue;

	return val;
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
	while (inputTokens.size() > 0) {
		// pop next token
		const string cmd = inputTokens.front();
		inputTokens.pop_front();

		// lookup command in command map
		if (!cmdMap.count(cmd)) {
			fprintf(stderr, "Unknown command %s\n", cmd.c_str());
			return false;
		}

		// arg count validation
		const commandInfo& cmdInfo = cmdMap[cmd];
		if (inputTokens.size() < cmdInfo.n_args) {
			fprintf(stderr, "Command %s missing arguments\n",
				cmd.c_str());
			return false;
		}

		// arg collection
		vector<string> cmdArgs;
		for (unsigned int i = 0; i < cmdInfo.n_args; i++) {
			const string tok = inputTokens.front();
			inputTokens.pop_front();
			cmdArgs.push_back(tok);
		}

		// per-command processing

		if (cmd == "get") {
			const UniValue& val = objectGet(cmdArgs[0]);
			jdoc = val;
		}
	}

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
	staticInit();

	// parse command line
	error_t argp_rc = argp_parse(&argp, argc, argv, 0, NULL, NULL);
	if (argp_rc) {
		fprintf(stderr, "%s: argp_parse failed: %s\n",
			argv[0], strerror(argp_rc));
		return EXIT_FAILURE;
	}

	if (doListCommands) {
		listCommands();
		return EXIT_SUCCESS;
	}

	if ((readStdin && !readInput()) ||
	    !processDocument() ||
	    !writeOutput())
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

