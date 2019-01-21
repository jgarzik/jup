
#include "jup-config.h"
#include <vector>
#include <deque>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <argp.h>
#include "univalue/include/univalue.h"
#include "utilstrencodings.h"
#include "utf8.h"

using namespace std;

#define PROGRAM_NAME "jup"

const char *argp_program_version =
PROGRAM_NAME " " VERSION;
static const char doc[] =
PROGRAM_NAME " - JSON swiss army knife";
static const char args_doc[] =
"EDIT-COMMANDS...";

static struct argp_option options[] = {
	{"min", 1001, 0, 0, "Minimize JSON output."},
	{"list-commands", 1002, 0, 0, "List all supported edit commands, and their descriptions."},
	{"list-short", 1006, 0, 0, "Short list of edit command usages."},
	{"indent", 1003, "NUM", 0, "Set JSON output indent spacing (0=disable). Overrides JUP_INDENT env var."},
	{"unhex", 1004, 0, 0, "If output is a simple string, perform hex-decode."},
	{"un64", 1005, 0, 0, "If output is a simple string, perform base64-decode."},

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

	{ 0, "new", "new",
	  "Create document with empty object.  stdin ignored.", true },
	{ 0, "newarray", "newarray",
	  "Create document with empty array.  stdin ignored.", true },

	{ 2, "str", "str JSON-PATH VALUE",
	  "Store VALUE at JSON-PATH" },
	{ 2, "int", "int JSON-PATH VALUE",
	  "Store integer VALUE at JSON-PATH" },
	{ 2, "num", "num JSON-PATH VALUE",
	  "Store floating point VALUE at JSON-PATH" },

	{ 1, "true", "true JSON-PATH",
	  "Store boolean true at JSON-PATH" },
	{ 1, "false", "false JSON-PATH",
	  "Store boolean false at JSON-PATH" },
	{ 1, "null", "null JSON-PATH",
	  "Store null at JSON-PATH" },

	{ 1, "array", "array JSON-PATH",
	  "Store empty array at JSON-PATH" },
	{ 1, "object", "object JSON-PATH",
	  "Store empty object at JSON-PATH" },

	{ 2, "file.text", "file.text JSON-PATH FILE",
	  "Store content of FILE at JSON-PATH" },
	{ 2, "file.json", "file.json JSON-PATH FILE",
	  "Store content of JSON FILE at JSON-PATH" },
	{ 2, "file.hex", "file.hex JSON-PATH FILE",
	  "Store (binary?) hex-encoded content of FILE at JSON-PATH" },
	{ 2, "file.base64", "file.base64 JSON-PATH FILE",
	  "Store (binary?) base64-encoded content of FILE at JSON-PATH" },
	{ 2, "file.csv", "file.csv JSON-PATH FILE",
	  "Decode and store CSV-formatted content of FILE at JSON-PATH" },
};

static error_t parse_opt (int key, char *arg, struct argp_state *state);
static const struct argp argp = { options, parse_opt, args_doc, doc };

static bool minimalJson = false;
static bool doListCommands = false;
static bool doListUsages = false;
static bool optDecode64 = false;
static bool optDecodeHex = false;
static int defaultIndent = 2;
UniValue jdoc(UniValue::VNULL);
map<string,commandInfo> cmdMap;
deque<string> inputTokens;

static void staticInit()
{
	for (unsigned int i = 0; i < ARRAYLEN(commandList); i++) {
		cmdMap[commandList[i].name] = commandList[i];
	}
}

static void listCommands(bool longForm)
{
	UniValue l(UniValue::VARR);
	UniValue shortl(UniValue::VARR);

	for (map<string,commandInfo>::const_iterator it = cmdMap.begin();
	     it != cmdMap.end(); it++) {
		UniValue obj(UniValue::VOBJ);

		const commandInfo& ci = it->second;

		obj.pushKV("command", ci.name);
		obj.pushKV("usage", ci.summary);
		obj.pushKV("help", ci.description);

		l.push_back(obj);
		shortl.push_back(ci.summary);
	}

	int wrIndent = minimalJson ? 0 : defaultIndent;
	string body = longForm ? l.write(wrIndent) : shortl.write(wrIndent);
	printf("%s\n", body.c_str());
}

static bool isDigitStr(const string& s)
{
	for (unsigned int i = 0; i < s.size(); i++)
		if (!isdigit(s[i]))
			return false;

	return true;
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

	case 1006:
		doListUsages = true;
		break;

	case 1003: {
		string indentStr(arg);
		if (isDigitStr(indentStr))
			defaultIndent = atoi(arg);
		break;
	}

	case 1004:
		optDecodeHex = true;
		optDecode64 = false;
		break;

	case 1005:
		optDecode64 = true;
		optDecodeHex = false;
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

static bool readBinaryFile(const string& filename, string& body)
{
	int fd = open(filename.c_str(), O_RDONLY);
	if (fd < 0) {
		perror(filename.c_str());
		return false;
	}

	bool rc = readStringFd(fd, body);
	close(fd);

	return rc;
}

static bool readTextFile(const string& filename, string& body)
{
	bool rc = readBinaryFile(filename, body);
	if (!rc)
		return false;

	return is_valid_utf8(body.c_str());
}

static bool readJsonFile(const string& filename, UniValue& jbody)
{
	string body;
	if (!readTextFile(filename, body))
		return false;

	if (!jbody.read(body)) {
		fprintf(stderr, "%s: JSON data not valid\n",
			filename.c_str());
		return false;
	}

	return true;
}

static bool readTextLines(const std::string& filename,
			  std::vector<std::string>& lines)
{
	char line[2048];

	FILE *f = fopen(filename.c_str(), "r");
	if (!f) {
		perror(filename.c_str());
		return false;
	}

	while (fgets(line, sizeof(line), f) != NULL)
		lines.push_back(line);

	if (ferror(f)) {
		perror(filename.c_str());
		return false;
	}

	fclose(f);

	return true;
}

static vector<string> ParseLineDelim(const string& line, char delim = ',',
				     bool parseQuotes = true)
{
	vector<string> data;
	string cur;
	size_t pos = 0;
	bool quoting = false;
	bool started = false;

	while (pos < line.size()) {
		char ch = line[pos++];

		if (quoting) {
			started = true;
			if (ch == '"')
				quoting = false;
			else
				cur.append(1, ch);
		} else {
			if ((ch == '"') && (parseQuotes)) {
				quoting = true;
				if (started)
					cur.append(1, ch);

			} else if (ch == delim) {
				data.push_back(cur);
				cur.clear();
				started = false;
			} else if (ch == '\r') {
				// ignore
			} else if (ch == '\n') {
				break;
			} else
				cur.append(1, ch);
		}
	}

	data.push_back(cur);

	return data;
}

static bool readDelimFile(const string& filename, UniValue& jbody)
{
	if (!jbody.isArray()) {
		fprintf(stderr, "Input not an array\n");
		return false;
	}

	vector<string> lines;

	if (!readTextLines(filename, lines))
		return false;

	for (const string& line : lines) {
		vector<string> cols = ParseLineDelim(line);
		UniValue arr(UniValue::VARR);

		for (const string& col : cols) {
			arr.push_back(col);
		}

		jbody.push_back(arr);
	}

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

static const UniValue& lookupPath(const string& path, deque<string>& rem,
				  bool& matched)
{
	rem.clear();
	matched = false;

	if (!is_valid_utf8(path.c_str()))
		return NullUniValue;

	deque<string> tokens;

	strsplit(path, ".", tokens);

	const UniValue* jptr = &jdoc;
	while (tokens.size() > 0) {
		const string& token = tokens.front();

		if (jptr->isObject() && jptr->exists(token)) {
			// direct path match
			if (tokens.size() == 1) {
				matched = true;
				return (*jptr)[token];
			}

			if (!(*jptr)[token].isObject() &&
			    !(*jptr)[token].isArray()) {
				rem = tokens;
				return *jptr;
			}

			jptr = &(*jptr)[token];
			tokens.pop_front();

		} else if (jptr->isArray() && isDigitStr(token) &&
			   ((unsigned long)atol(token.c_str()) < jptr->size())) {

			size_t index = atol(token.c_str());
			// direct path match
			if (tokens.size() == 1) {
				matched = true;
				return (*jptr)[index];
			}

			if (!(*jptr)[index].isObject() &&
			    !(*jptr)[index].isArray()) {
				rem = tokens;
				return *jptr;
			}

			jptr = &(*jptr)[index];
			tokens.pop_front();
		} else if (jptr->isArray() && !isDigitStr(token)) {
			return NullUniValue;
		} else {
			rem = tokens;
			return *jptr;
		}
	}

	return *jptr;
}

const UniValue& jdocGet(const string& jpath)
{
	deque<string> rem;
	bool matched;

	const UniValue& val = lookupPath(jpath, rem, matched);
	if (!matched)
		return NullUniValue;

	return val;
}

static bool jdocSet(const string& jpath, const UniValue& jval)
{
	deque<string> rem;
	bool matched;

	UniValue& container =
		(UniValue&) lookupPath(jpath, rem, matched);

	if (container.isNull()) {
		fprintf(stderr, "Invalid json path\n");
		return false;
	}
	if (matched) {
		fprintf(stderr, "TODO: overwriting values not yet supported\n");
		return false;
	}
	if (rem.size() > 1) {
		fprintf(stderr, "Cannot find json path\n");
		return false;
	}
	assert(rem.size() == 1);	// TODO: create intermediate path objs

	if (container.isObject()) {
		const string& key = rem.front();
		container.pushKV(key, jval);

	} else if (container.isArray()) {
		const string& indexStr = rem.front();
		if (!isDigitStr(indexStr)) {
			fprintf(stderr,"Invalid array index\n");
			return false;
		}
		unsigned int index = (unsigned int) atoi(indexStr.c_str());

		assert(index >= container.size());

		// fill in sparse arrays
		while (index > container.size())
			container.push_back(NullUniValue);

		assert(index == container.size());

		// add new item
		container.push_back(jval);
	}

	else {
		assert(0 && "Unexpected container type");
	}

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
			assert(cmdArgs.size() == 1);
			UniValue val = jdocGet(cmdArgs[0]);
			jdoc = val;
		}

		else if (cmd == "new") {
			assert(cmdArgs.size() == 0);
			UniValue tmp(UniValue::VOBJ);
			jdoc = tmp;
		}

		else if (cmd == "newarray") {
			assert(cmdArgs.size() == 0);
			UniValue tmp(UniValue::VARR);
			jdoc = tmp;
		}

		else if (cmd == "str") {
			assert(cmdArgs.size() == 2);
			const string& jpath = cmdArgs[0];
			const string& val = cmdArgs[1];

			if (!is_valid_utf8(val.c_str())) {
				fprintf(stderr, "string not UTF8: %s\n",
					val.c_str());
				return false;
			}

			UniValue jval(val);
			if (!jdocSet(jpath, jval))
				return false;
		}

		else if (cmd == "int") {
			assert(cmdArgs.size() == 2);
			const string& jpath = cmdArgs[0];
			const string& valStr = cmdArgs[1];

			errno = 0;
			unsigned long long l = strtoull(valStr.c_str(),
							NULL, 10);
			if (errno != 0) {
				fprintf(stderr, "integer parse failed: %s\n",
					valStr.c_str());
				return false;
			}

			UniValue jval((uint64_t) l);
			if (!jdocSet(jpath, jval))
				return false;
		}

		else if (cmd == "num") {
			assert(cmdArgs.size() == 2);
			const string& jpath = cmdArgs[0];
			const string& valStr = cmdArgs[1];

			errno = 0;
			double d = strtold(valStr.c_str(), NULL);
			if (errno != 0) {
				fprintf(stderr, "number parse failed: %s\n",
					valStr.c_str());
				return false;
			}

			UniValue jval(d);
			if (!jdocSet(jpath, jval))
				return false;
		}

		else if (cmd == "true" || cmd == "false" || cmd == "null") {
			assert(cmdArgs.size() == 1);
			UniValue jval;
			if (cmd == "true")	jval.setBool(true);
			else if (cmd == "false") jval.setBool(false);

			if (!jdocSet(cmdArgs[0], jval))
				return false;
		}

		else if (cmd == "array" || cmd == "object") {
			assert(cmdArgs.size() == 1);
			UniValue jval(cmd == "object" ? UniValue::VOBJ : UniValue::VARR);

			if (!jdocSet(cmdArgs[0], jval))
				return false;
		}

		else if (cmd == "file.text") {
			assert(cmdArgs.size() == 2);
			const string& jpath = cmdArgs[0];
			const string& filename = cmdArgs[1];
			string body;

			if (!readTextFile(filename, body))
				return false;

			UniValue jval(body);
			if (!jdocSet(jpath, jval))
				return false;
		}

		else if (cmd == "file.csv") {
			assert(cmdArgs.size() == 2);
			const string& jpath = cmdArgs[0];
			const string& filename = cmdArgs[1];
			UniValue jbody(UniValue::VARR);

			if (!readDelimFile(filename, jbody) ||
			    !jdocSet(jpath, jbody))
				return false;
		}

		else if (cmd == "file.json") {
			assert(cmdArgs.size() == 2);
			const string& jpath = cmdArgs[0];
			const string& filename = cmdArgs[1];
			UniValue jbody;

			if (!readJsonFile(filename, jbody) ||
			    !jdocSet(jpath, jbody))
				return false;
		}

		else if (cmd == "file.hex" || cmd == "file.base64") {
			assert(cmdArgs.size() == 2);
			const string& jpath = cmdArgs[0];
			const string& filename = cmdArgs[1];
			string rawBody;

			if (!readBinaryFile(filename, rawBody))
				return false;

			string body;
			if (cmd == "file.hex")
				body = HexStr(rawBody.begin(), rawBody.end());
			else
				body = EncodeBase64(rawBody);

			UniValue jval(body);
			if (!jdocSet(jpath, jval))
				return false;
		}

		else {
			// should never be reached
			assert(0 && "unhandled command trap");
		}
	}

	return true;
}

static bool writeOutput()
{
	if (jdoc.isStr()) {
		const string& val = jdoc.getValStr();

		if (optDecodeHex) {
			vector<unsigned char> buf = ParseHex(val);
			if (buf.size() != (val.size() / 2)) {
				fprintf(stderr, "Hex decode failed\n");
				return false;
			}

			// todo: remove unneeded buffer copy

			string bufStr(buf.begin(), buf.end());
			return writeStringFd(STDOUT_FILENO, bufStr);
		} else if (optDecode64) {
			bool invalid = false;
			vector<unsigned char> buf =
				DecodeBase64(val.c_str(), &invalid);
			if (invalid) {
				fprintf(stderr, "Base64 decode failed\n");
				return false;
			}

			// todo: remove unneeded buffer copy

			string bufStr(buf.begin(), buf.end());
			return writeStringFd(STDOUT_FILENO, bufStr);

		} else {
			return writeStringFd(STDOUT_FILENO, val);
		}
	}

	string rawBody = jdoc.write(minimalJson ? 0 : defaultIndent);
	rawBody.append("\n");

	return writeStringFd(STDOUT_FILENO, rawBody);
}

static bool ignoreStdin()
{
	const string& firstCmd = inputTokens.size() ? inputTokens[0] : "";

	if (cmdMap.count(firstCmd) > 0) {
		const commandInfo& ci = cmdMap[firstCmd];
		if (ci.ignoreStdin)
			return true;
	}

	return false;
}

static void envInit()
{
	const char *envar;

	if ((envar = getenv("JUP_INDENT")) != nullptr) {
		string indentStr(envar);
		if (isDigitStr(indentStr))
			defaultIndent = atoi(envar);
	}
}

int main (int argc, char *argv[])
{
	staticInit();
	envInit();

	// parse command line
	error_t argp_rc = argp_parse(&argp, argc, argv, 0, NULL, NULL);
	if (argp_rc) {
		fprintf(stderr, "%s: argp_parse failed: %s\n",
			argv[0], strerror(argp_rc));
		return EXIT_FAILURE;
	}

	if (doListCommands || doListUsages) {
		listCommands(doListCommands ? true : false);
		return EXIT_SUCCESS;
	}

	if ((!ignoreStdin() && !readInput()) ||
	    !processDocument() ||
	    !writeOutput())
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

