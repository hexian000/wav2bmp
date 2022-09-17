#include "wav2bmp.hpp"
#include "sndfile.hpp"
#include "fft.hpp"
#include "spectrum.hpp"
#include "thread_pool.hpp"
#include "wav2bmp_error.hpp"

#include <cmath>
#include <iostream>

using namespace std;

wav2bmp::wav2bmp(const options &op) {
	this->op = op;
}

[[maybe_unused]] static void print_norm(const string &msg, const double *buf, size_t size) {
	double norm = 0;
	for (size_t i = 0; i < size; i++) {
		norm += buf[i] * buf[i];
	}
	cerr << msg << ": norm=" << norm << endl;
}

void wav2bmp::run() const {
	sndfile in(op.input);
	const auto channels = in.channels();
	const auto length = in.length();
	const auto sample_rate = in.sample_rate();
	const auto n = op.size;
	const auto cut_down_freq_hz = min(op.cut_down_frequency, sample_rate / 2.0);
	const auto start_freq_hz = min(op.start_frequency, cut_down_freq_hz - 1.0);
	const auto logarithmic = op.logarithmic;
	const auto height = logarithmic > 0.0
						? static_cast<unsigned>(std::floor((cut_down_freq_hz - start_freq_hz) * logarithmic))
						: static_cast<unsigned>(std::floor((cut_down_freq_hz - start_freq_hz) / sample_rate * n));
	const auto dist = static_cast<unsigned>(std::floor(sample_rate / op.resolution));
	const auto count = static_cast<unsigned>((length - n) / dist) + 1u;

	if (length < n || height < 16u || count < 16u) {
		throw wav2bmp_error("sound file is too short or improper arguments");
	}

	cerr << op.input << " => " << op.output << endl;
	cerr << "sample rate: " << sample_rate << ", channels: " << channels << endl;
	cerr << "n=" << n << ", dist=" << dist << endl;

	double *raw;
	{
		const std::size_t to_read = dist * (count - 1) + n;
		raw = new double[to_read * channels];
		cerr << "read " << to_read << " frames" << endl;
		const auto begin = chrono::steady_clock::now();
		const auto read = in.read(raw, to_read);
		const auto end = chrono::steady_clock::now();
		cerr << " ... " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << " ms" << endl;
		if (read != to_read) {
			throw wav2bmp_error("early eof");
		}
	}

	spectrum s(n, height, count, channels, op.brightness,
			   start_freq_hz / sample_rate * n,
			   cut_down_freq_hz / sample_rate * n,
			   sample_rate, logarithmic);
	{
		fft transform(n);
		const auto begin = chrono::steady_clock::now();
		thread_pool executor(thread::hardware_concurrency());
		for (int ch = 0; ch < channels; ch++) {
			cerr << "channel " << ch << ": compute " << count << " " << n << "-point FFTs" << endl;
			for (unsigned j = 0; j < count; j++) {
				executor.push([&transform, &raw, &s, n, j, dist, channels, ch, logarithmic]() {
					auto buf = transform.get_buffer();
					for (unsigned k = 0; k < n; k++) {
						buf[k] = raw[(j * dist + k) * channels + ch];
					}
					transform.compute(buf);
					if (logarithmic > 0.0) {
						s.paint_log(buf, j, ch);
					} else {
						s.paint(buf, j, ch);
					}
					transform.put_buffer(buf);
				});
			}
		}
		executor.join();
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
