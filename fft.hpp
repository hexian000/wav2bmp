#ifndef WAV2BMP_FFT_HPP
#define WAV2BMP_FFT_HPP

#include <vector>
#include <memory>
#include <stack>

struct buffer_pool;
struct fft_private;

class fft {
  const int batch, n, dist, channels;
  std::unique_ptr<struct fft_private> p;

 public:
  explicit fft(int batch, int n, int dist, int channels);

  ~fft();

  void compute(double *);

  double *get_buffer();

  void put_buffer(double *);
};

#endif //WAV2BMP_FFT_HPP
