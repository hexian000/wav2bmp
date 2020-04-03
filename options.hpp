#ifndef WAV2BMP_OPTIONS_HPP
#define WAV2BMP_OPTIONS_HPP

#include <string>

struct options {
  std::string input, output;
  unsigned size;
  double resolution, brightness;
  double cut_down_frequency;
  double start_frequency;

  options()
      : size(4096), resolution(16.0), brightness(1e-3),
        cut_down_frequency(440.0 * 32.0),
        start_frequency(440.0 / 32.0) {}

  void check();
};

#endif //WAV2BMP_OPTIONS_HPP
