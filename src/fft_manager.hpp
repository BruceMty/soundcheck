#ifndef FFT_MANAGER_HPP
#define FFT_MANAGER_HPP

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "libfft.h"

class fft_manager_t {
  private:
  const int fft_exp_size;
  void* fft;

  fft_manager_t(const fft_manager_t&);
  fft_manager_t& operator=(const fft_manager_t&);

  public:
  fft_manager_t(int p_fft_exp_size) :
             fft_exp_size(p_fft_exp_size), fft(NULL) {
    fft = initfft(fft_exp_size);
  }

  ~fft_manager_t() {
    destroyfft(fft);
  }

  void apply_fft(float* d_r, float* d_i) {
    applyfft(fft, d_r, d_i, false);
  }
};

#endif

