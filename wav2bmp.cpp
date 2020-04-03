#include "wav2bmp.hpp"
#include "sndfile.hpp"
#include "fft.hpp"
#include "spectrum.hpp"
#include "thread_pool.hpp"

#include <mutex>
#include <iostream>
#include <vector>

using namespace std;

wav2bmp::wav2bmp(const options &op) {
  this->op = op;
}

static void print_norm(const string &msg, const double *buf, size_t size) {
  double norm = 0;
  for (size_t i = 0; i < size; i++) {
    norm += buf[i] * buf[i];
  }
  cerr << msg << ": norm=" << norm << endl;
}

void wav2bmp::run() {
  sndfile in(op.input);
  const int channels = in.channels();
  const size_t length = in.length();
  const int sample_rate = in.sample_rate();
  const int n = static_cast<int>(op.size);
  const int height = static_cast<int>(op.cut_down_frequency / sample_rate * n);
  const int dist = static_cast<int>(sample_rate / op.resolution);
  const int count = static_cast<int>((length - n) / dist) + 1;

  cerr << op.input << " => " << op.output << endl;
  cerr << "sample rate: " << sample_rate << ", channels: " << channels << endl;
  cerr << "n=" << n << ", dist=" << dist << endl;

  double *raw;
  {
    const auto to_read = dist * (count - 1) + n;
    raw = new double[to_read * channels];
    cerr << "read " << to_read << " frames" << endl;
    const auto begin = chrono::steady_clock::now();
    const auto read = in.read(raw, to_read);
    const auto end = chrono::steady_clock::now();
    cerr << " ... " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms" << endl;
    if (read != to_read) {
      cerr << "eof" << endl;
      return;
    }
  }

  const double start_freq = op.start_frequency / sample_rate * n;
  spectrum s(n, height, count, channels, op.brightness, start_freq);
  fft transform(count, n, dist, channels);
  {
    const auto begin = chrono::steady_clock::now();
    thread_pool executor(thread::hardware_concurrency());
    for (int ch = 0; ch < channels; ch++) {
      cerr << "channel " << ch << ": compute " << count << " " << n << "-point FFTs" << endl;
      for (int j = 0; j < count; j++) {
        executor.push([&transform, &raw, &s, n, j, dist, channels, ch]() {
          auto buf = transform.get_buffer();
          for (int k = 0; k < n; k++) {
            buf[k] = raw[(j * dist + k) * channels + ch];
          }
          transform.compute(buf);
          s.paint(buf, j, ch);
          transform.put_buffer(buf);
        });
      }
    }
    executor.shutdown();
    const auto end = chrono::steady_clock::now();
    cerr << " ... " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms" << endl;
  }

  {
    cerr << "write " << count << "x" << height * channels << " image" << endl;
    const auto begin = chrono::steady_clock::now();
    s.write(op.output);
    const auto end = chrono::steady_clock::now();
    cerr << " ... " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms" << endl;
  }
  cerr << "done" << endl;
}
