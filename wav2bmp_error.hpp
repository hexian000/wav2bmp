#ifndef WAV2BMP_WAV2BMP_ERROR_HPP
#define WAV2BMP_WAV2BMP_ERROR_HPP

#include <stdexcept>

class wav2bmp_error : public std::runtime_error {
public:
	explicit wav2bmp_error(const std::string &what) : std::runtime_error(what) {}

	explicit wav2bmp_error(const char *what) : std::runtime_error(what) {}
};

#endif  // WAV2BMP_WAV2BMP_ERROR_HPP
