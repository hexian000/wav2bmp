#ifndef WAV2BMP_FFT_HPP
#define WAV2BMP_FFT_HPP

#include <vector>
#include <memory>
#include <stack>

class buffer_pool;
struct fft_private;

class fft {
	const unsigned int n;
	std::unique_ptr<struct fft_private> p;

public:
	explicit fft(unsigned int n);

	~fft();

	void compute(double *);

	double *get_buffer();

	void put_buffer(double *);
};

#endif //WAV2BMP_FFT_HPP
