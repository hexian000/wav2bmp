#include "options.hpp"
#include "option_error.hpp"

#include <boost/format.hpp>

#include <cmath>

static inline bool is_sane(double x) {
  return !(std::isnan(x) || std::isinf(x));
}

void options::check() {
  using namespace std;

#define THROW_INVALID(x) throw option_error((boost::format("invalid value for '" #x "' - %d") % (x)).str())

  if ((size< 64) || (size != static_cast<unsigned>(pow(2, floor(log2(size)))))) {
    THROW_INVALID(size);
  }

  if (!is_sane(resolution) || (resolution <= 0.0)) {
    THROW_INVALID(resolution);
  }

  if (!is_sane(brightness) || (brightness <= 0.0)) {
    THROW_INVALID(brightness);
  }

  if (!is_sane(cut_down_frequency) || (cut_down_frequency <= 0.0)) {
    THROW_INVALID(cut_down_frequency);
  }

  if (!is_sane(start_frequency) || (start_frequency <= 0.0)) {
    THROW_INVALID(start_frequency);
  }

#undef THROW_INVALID
}
