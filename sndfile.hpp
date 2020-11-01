#ifndef WAV2BMP_SNDFILE_HPP
#define WAV2BMP_SNDFILE_HPP

#include <string>
#include <memory>
#include <vector>

struct sndfile_private;

class sndfile {
	std::unique_ptr<struct sndfile_private> p;

public:
	explicit sndfile(const std::string &path);

	~sndfile();

	int channels();

	int sample_rate();

	std::size_t length();

	std::size_t read(double *buf, std::size_t n);
};

#endif //WAV2BMP_SNDFILE_HPP
