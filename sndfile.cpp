#include "sndfile.hpp"
#include "wav2bmp_error.hpp"

#include <sndfile.h>

struct sndfile_private {
  SF_INFO info;
  SNDFILE *f;
};

sndfile::sndfile(const std::string &path) {
  p = std::make_unique<sndfile_private>();
  p->f = sf_open(path.c_str(), SFM_READ, &(p->info));
  if (p->f == nullptr) {
    throw wav2bmp_error(path + ": " + sf_strerror(p->f));
  }
}

sndfile::~sndfile() {
  if (p->f) {
    sf_close(p->f);
  }
}

std::size_t sndfile::read(double *buf, std::size_t n) {
  return sf_readf_double(p->f, buf, n);
}

int sndfile::channels() {
  return p->info.channels;
}

std::size_t sndfile::length() {
  return p->info.frames;
}

int sndfile::sample_rate() {
  return p->info.samplerate;
}
