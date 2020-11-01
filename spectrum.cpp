#include "spectrum.hpp"

#include <png++/png.hpp>

#include <cmath>
#include <complex>

using namespace std;

constexpr double png_gamma = 1.0 / 2.4;

struct spectrum_private {
	int n, height;
	png::image<png::rgb_pixel> image;

	spectrum_private(const int n, const int height, const int length, const int channels)
			: n(n), height(height), image(length, height * channels) {
		image.set_gamma(png_gamma);
	}
};

static inline double clamp(double v) {
	if (v <= 0.0) {
		return 0.0;
	} else if (v >= 1.0) {
		return 1.0;
	}
	return v;
}

static inline int clamp(int v, int min, int max) {
	if (v < min) {
		return min;
	} else if (v > max) {
		return max;
	}
	return v;
}

spectrum::spectrum(const int n, const int height, const int length, const int channels, const double brightness,
				   const double start_freq, const double cut_freq, const double sample_rate,
				   const double vertical_resolution)
		: p(std::make_unique<spectrum_private>(n, height, length, channels)),
		  brightness(brightness), start_freq(start_freq), cut_freq(cut_freq), sample_rate(sample_rate),
		  vertical_resolution(vertical_resolution) {
}

spectrum::~spectrum() = default;

typedef std::tuple<double, double, double> rgb_t;

static inline rgb_t palette(double v) {
	double r = clamp(v * 400), g = clamp(v * 20), b = clamp(v);
	return {r, g, b};
}

static inline uint8_t to_byte(double v) {
	if (v < 0.0) {
		return 0u;
	}
	v *= 255.0;
	if (v > 255.0) {
		return 255u;
	}
	return static_cast<uint8_t>(v);
}

static inline uint8_t to_color(double v) {
	return to_byte(pow(v, png_gamma));
}

static inline png::rgb_pixel make_pixel(const double k, const std::complex<double> v) {
	const auto[r, g, b] = palette(k * std::abs(v));
	return png::rgb_pixel(to_color(r), to_color(g), to_color(b));
}

void spectrum::paint(const double *hc, const int x, const int ch) {
	const auto n = p->n;
	const auto height = p->height;
	auto &image = p->image;
	for (int y = 0; y < height; ++y) {
		const auto freq = static_cast<int>(floor(y + start_freq));
		const auto i = clamp(freq, 0, n - 1);
		const auto v = std::complex<double>(hc[i + 1], hc[n - 1 - i]);
		image[(ch * height) + (height - 1 - y)][x] = make_pixel(brightness, v);
	}
}

void spectrum::paint_log(const double *hc, const int x, const int ch) {
	const auto n = p->n;
	const auto height = p->height;
	const auto start_tune = log2(start_freq);
	const auto ratio = log2(cut_freq) - start_tune;
	auto &image = p->image;
	for (int y = 0; y < height; ++y) {
		const auto tune = ((y + 1.0) / height * ratio + start_tune);
		const auto freq = static_cast<int>(floor(pow(2.0, tune)));
		const auto i = clamp(freq, 0, n - 1);
		const auto v = std::complex<double>(hc[i + 1], hc[n - 1 - i]);
		image[(ch * height) + (height - 1 - y)][x] = make_pixel(brightness, v);
	}
}

void spectrum::write(const std::string &path) {
	p->image.write(path);
}
