#ifndef WAV2BMP_WAV2BMP_HPP
#define WAV2BMP_WAV2BMP_HPP

#include "options.hpp"

class wav2bmp {
	options op;

public:
	explicit wav2bmp(const options &);

	void run() const;
};

#endif //WAV2BMP_WAV2BMP_HPP
