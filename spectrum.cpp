#include "spectrum.hpp"

#include <png++/png.hpp>

#include <cstdint>
#include <cmath>
#include <tuple>

using namespace std;

struct spectrum_private {
  int n, height, length, channels;
  png::image<png::rgb_pixel> image;

  spectrum_private(const int n, const int height, const int length, const int channels)
      : n(n), height(height), length(length), channels(channels),
        image(length, height * channels) {}
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
  if (v <= min) {
    return min;
  } else if (v >= max) {
    return max;
  }
  return v;
}

spectrum::spectrum(const int n, const int height, const int length, const int channels, double brightness,
                   double min_tune)
    : p(std::make_unique<spectrum_private>(n, height, length, channels)),
      brightness(brightness), start_freq(min_tune) {
}

spectrum::~spectrum() = default;

typedef std::tuple<double, double, double> rgb_t;

static inline rgb_t palette(double v) {
  double r = clamp(v * 400), g = clamp(v * 20), b = clamp(v);
  return {r, g, b};
}

static inline uint8_t to_byte(double v) {
  if (v <= 0.0) {
    return 0u;
  }
  v *= 255.0;
  if (v >= 255.0) {
    return 255u;
  }
  return static_cast<uint8_t>(v);
}

static inline uint8_t to_sRGB(double v) {
  if (v <= 0.0031308) {
    return to_byte(v * 12.92);
  }
  return to_byte(pow(v, 1.0 / 2.4) * 1.055 - 0.055);
}

static inline png::rgb_pixel make_pixel(const double k, const double real, const double imag) {
  const double A = hypot(real, imag);
  const double v = k * A;
  const auto[r, g, b] = palette(v);
  return png::rgb_pixel(to_sRGB(r), to_sRGB(g), to_sRGB(b));
}

void spectrum::paint(const double *hc, const int x, const int ch) {
  const double k = brightness;
  const int n = p->n;
  const int height = p->height;
  const double max_tune = log2(height);
  for (int y = 0; y < height; ++y) {
    const int i = clamp((int) (pow(2, y * max_tune / height) + start_freq), 0, n - 1);
    const double real = hc[i + 1];
    const double imag = hc[n - 1 - i];
    p->image[(ch * height) + (height - 1 - y)][x] = make_pixel(k, real, imag);
  }
}

void spectrum::write(const std::string &path) {
  p->image.write(path);
}
