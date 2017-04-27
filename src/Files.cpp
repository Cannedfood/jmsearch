#include "Files.hpp"

#include <fstream>
#include <zlib.h>

#ifdef __unix__
#	include <dlfcn.h>

class Gzip {
private:
	void* mHandle = nullptr;
public:
	decltype(gzopen)*  open  = nullptr;
	decltype(gzclose)* close = nullptr;
	decltype(gzeof)*   eof   = nullptr;
	decltype(gzread)*  read  = nullptr;

	Gzip() {
#define GET(fn) (decltype(fn)*) dlsym(mHandle, #fn)
		if((mHandle = dlopen("libz.so", RTLD_GLOBAL | RTLD_LAZY))) {
			open  = GET(gzopen);
			close = GET(gzclose);
			eof   = GET(gzeof);
			read  = GET(gzread);
		}
	}

	~Gzip() {
		if(mHandle)
			dlclose(mHandle);
	}
};
#else
// TODO: windows support
// TODO: mac support
#	error "TODO: implement for non-posix"
#endif

static
bool loadFileIntoVectorZlib(const std::string& path, std::vector<char>& to) {
	Gzip gz;
	if(!gz.open) {
		puts("zlib unavailable...");
		return false;
	}

	gzFile file = gz.open(path.c_str(), "r");
	if(!file) {
		printf("Couldn't open '%s'\n", path.c_str());
		return false;
	}

	std::vector<std::vector<char>> buffers;

	while(!gz.eof(file)) {
		buffers.emplace_back(2 << 14);
		buffers.back().resize( // Resize the latest buffer to...
			gz.read( // ...the size of the data read to it
				file,
				buffers.back().data(),
				buffers.back().size()
			)
		);
	}

	gz.close(file);

	{
		size_t total = 0;
		for(const auto& v : buffers) total += v.size();
		to.resize(total + 1);
	}

	auto out_iter = to.begin();
	for(auto& v : buffers) {
		out_iter = std::copy(v.begin(), v.end(), out_iter);
		v.clear();
	}
	to[to.size() - 1] = '\0';

	return true;
}

static
bool loadFileIntoVectorDefault(const std::string& path, std::vector<char>& to) {
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if(!file) return false;

	to.resize(((long)file.tellg()) + 1);
	file.seekg(std::ios::beg);
	file.read(to.data(), to.size() - 1);
	to[to.size() - 1] = '\0';

	return true;
}

bool loadFileIntoVector(const std::string& path, std::vector<char>& to) {
	if(!loadFileIntoVectorZlib(path + ".gz", to)) {
		if(!loadFileIntoVectorDefault(path + ".xml", to)) {
			printf("Neither '%s.xml' nor '%s.gz' could be loaded.\n", path.c_str(), path.c_str());
			return false;
		}
	}

	return true;
}
