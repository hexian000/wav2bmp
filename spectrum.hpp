#ifndef WAV2BMP_SPECTRUM_HPP
#define WAV2BMP_SPECTRUM_HPP

#include <string>
#include <vector>
#include <memory>

struct spectrum_private;

class spectrum {
  std::unique_ptr<struct spectrum_private> p;
  const double brightness, start_freq;

 public:
  spectrum(int n, int height, int length, int channels, double brightness, double min_tune);

  ~spectrum();

  void paint(const double *hc, int x, int channel);

  void write(const std::string &path);
};

#endif //WAV2BMP_SPECTRUM_HPP
