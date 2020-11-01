#include "fft.hpp"

#include <fftw3.h>

#include <cmath>
#include <mutex>

using namespace std;

class buffer_pool {
	const size_t size;
	mutable mutex mu;
	stack<double *> buffer;

public:
	explicit buffer_pool(const size_t size) : size(size) {}

	inline double *get() {
		lock_guard<mutex> l(mu);
		if (!buffer.empty()) {
			double *buf = buffer.top();
			buffer.pop();
			return buf;
		}
		return fftw_alloc_real(size);
	}

	inline void put(double *buf) {
		lock_guard<mutex> l(mu);
		buffer.push(buf);
	}

	~buffer_pool() {
		lock_guard<mutex> l(mu);
		while (!buffer.empty()) {
			fftw_free(buffer.top());
			buffer.pop();
		}
	}
};

static inline double Blackman_Harris(double x) {
	constexpr double a0 = 0.35875, a1 = 0.48829, a2 = 0.14128, a3 = 0.01168;
	const double y = a0 - a1 * cos(2.0 * M_PI * x) + a2 * cos(4.0 * M_PI * x) - a3 * cos(6.0 * M_PI * x);
	return y;
}

struct fft_private {
	const std::vector<double> window;
	fftw_plan plan;
	buffer_pool pool;

	explicit fft_private(const unsigned int n) : window(n), pool(n) {
		const auto buf = pool.get();
		plan = fftw_plan_r2r_1d(static_cast<int>(n), buf, buf, FFTW_R2HC, 0);
		pool.put(buf);
		const auto divider = static_cast<double>(n - 1);
		auto &w = const_cast<std::vector<double> &>(window);
		for (int i = 0; i < n; i++) {
			w[i] = Blackman_Harris(static_cast<double>(i) / divider);
		}
	}

	~fft_private() {
		fftw_destroy_plan(plan);
	}
};

fft::fft(unsigned int n)
		: n(n), p(std::make_unique<fft_private>(n)) {
}

fft::~fft() {
	fftw_cleanup();
}

void fft::compute(double *buf) {
	const auto &w = p->window;
	for (int i = 0; i < n; i++) {
		buf[i] *= w[i];
	}
	fftw_execute_r2r(p->plan, buf, buf);
}

double *fft::get_buffer() {
	return p->pool.get();
}

void fft::put_buffer(double *buf) {
	p->pool.put(buf);
}
