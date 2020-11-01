#ifndef WAV2BMP_SPECTRUM_HPP
#define WAV2BMP_SPECTRUM_HPP

#include <string>
#include <vector>
#include <memory>

struct spectrum_private;

class spectrum {
	std::unique_ptr<struct spectrum_private> p;
	const double brightness, start_freq, cut_freq, sample_rate, vertical_resolution;

public:
	spectrum(int n, int height, int length, int channels, double brightness,
			 double start_freq, double cut_freq, double sample_rate,
			 double vertical_resolution);

	~spectrum();

	void paint(const double *hc, int x, int channel);

	void paint_log(const double *hc, int x, int channel);

	void write(const std::string &path);
};

#endif //WAV2BMP_SPECTRUM_HPP
