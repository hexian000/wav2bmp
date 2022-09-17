#include "spectrum.hpp"

#include <png++/png.hpp>

#include <algorithm>
#include <limits>
#include <cmath>
#include <complex>

constexpr double png_gamma = 1.0 / 2.4;

struct spectrum_private {
	int n, height;
	png::image<png::rgb_pixel> image;

	spectrum_private(const int n, const int height, const int length, const int channels)
			: n(n), height(height), image(length, height * channels) {
		image.set_gamma(png_gamma);
	}
};

spectrum::spectrum(const int n, const int height, const int length, const int channels, const double brightness,
				   const double start_freq, const double cut_freq, const double sample_rate,
				   const double vertical_resolution)
		: p(std::make_unique<spectrum_private>(n, height, length, channels)),
		  brightness(brightness), start_freq(start_freq), cut_freq(cut_freq), sample_rate(sample_rate),
		  vertical_resolution(vertical_resolution) {
}

spectrum::~spectrum() = default;

typedef std::tuple<double, double, double> rgb_t;

static inline rgb_t palette(const double v) {
	const auto r = std::clamp(v * 400, 0.0, 1.0);
	const auto g = std::clamp(v * 20, 0.0, 1.0);
	const auto b = std::clamp(v, 0.0, 1.0);
	return {r, g, b};
}

template<typename int_type, typename float_type>
static inline int_type to_int(float_type v) {
	if (std::isnan(v)) {
		return int_type();
	}
	v = std::clamp(v,
				   static_cast<float_type>(std::numeric_limits<int_type>::min()),
				   static_cast<float_type>(std::numeric_limits<int_type>::max()));
	return static_cast<int_type>(v);
}

static inline uint8_t to_color(double v) {
	return to_int<uint8_t, double>(std::pow(v, png_gamma) * 255.0);
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
		const auto freq = static_cast<int>(std::floor(y + start_freq));
		const auto i = std::clamp(freq, 0, n - 1);
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
		const auto freq = static_cast<int>(std::floor(std::pow(2.0, tune)));
		const auto i = std::clamp(freq, 0, n - 1);
		const auto v = std::complex<double>(hc[i + 1], hc[n - 1 - i]);
		image[(ch * height) + (height - 1 - y)][x] = make_pixel(brightness, v);
	}
}

void spectrum::write(const std::string &path) {
	p->image.write(path);
}
