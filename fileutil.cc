
#include "jup-config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include "fileutil.h"
#include "utf8.h"

using namespace std;

static const int INPUT_BUFSIZE = 8192;

bool readStringFd(int fd, string& rawBody)
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

bool writeStringFd(int fd, const string& rawBody)
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

bool readBinaryFile(const string& filename, string& body)
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

bool readTextFile(const string& filename, string& body)
{
	bool rc = readBinaryFile(filename, body);
	if (!rc)
		return false;

	return is_valid_utf8(body.c_str());
}

