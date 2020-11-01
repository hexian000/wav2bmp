#ifndef WAV2BMP_OPTION_ERROR_HPP
#define WAV2BMP_OPTION_ERROR_HPP

#include <stdexcept>

class option_error : public std::runtime_error {
public:
	explicit option_error(const std::string &what) : std::runtime_error(what) {}

	explicit option_error(const char *what) : std::runtime_error(what) {}
};

#endif //WAV2BMP_OPTION_ERROR_HPP
