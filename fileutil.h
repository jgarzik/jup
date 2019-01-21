#ifndef __FILEUTIL_H__
#define __FILEUTIL_H__

#include <string>
#include <vector>
#include <cassert>
#include <cstdio>

class RFile {
private:
	FILE	*f;

public:
	RFile() : f(nullptr) {}
	~RFile() { close(); }

	bool haveError() { return ferror(f); }
	bool haveEOF() { return feof(f); }

	bool open(const std::string& filename, const std::string& mode)
	{
		f = fopen(filename.c_str(), mode.c_str());
		if (!f)
			return false;

		return true;
	}

	void close() {
		if (f) {
			fclose(f);
			f = nullptr;
		}
	}

	size_t read(size_t sz, std::vector<unsigned char>& buf) {
		if (!f)
			return false;

		if (sz > buf.size())
			sz = buf.size();

		if (sz == 0)
			return true;

		return fread(&buf[0], 1, sz, f);
	}

	size_t write(size_t sz, const std::vector<unsigned char>& buf) {
		if (!f)
			return false;

		if (sz > buf.size())
			sz = buf.size();

		if (sz == 0)
			return true;

		return fwrite(&buf[0], 1, sz, f);
	}

	bool getline(std::string& line) {
		if (!f)
			return false;

		char linebuf[1024];

		char *rc = fgets(linebuf, sizeof(linebuf), f);
		if (!rc)
			return false;

		line.assign(rc);

		return true;
	}
};

extern bool readStringFd(int fd, std::string& rawBody);
extern bool writeStringFd(int fd, const std::string& rawBody);
extern bool readBinaryFile(const std::string& filename, std::string& body);
extern bool readTextFile(const std::string& filename, std::string& body);
extern bool readTextLines(const std::string& filename, std::vector<std::string>& lines);

#endif // __FILEUTIL_H__
